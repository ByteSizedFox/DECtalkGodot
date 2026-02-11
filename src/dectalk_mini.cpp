#include "dectalk_mini.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/char_string.hpp>

#include <chrono>
#include <cstring>

extern "C" {
#include "epsonapi.h"
}

namespace godot {

DECtalkMini *DECtalkMini::active_instance = nullptr;

DECtalkMini::DECtalkMini() = default;

DECtalkMini::~DECtalkMini() {
    if (active_instance == this) {
        active_instance = nullptr;
    }
}

void DECtalkMini::_bind_methods() {
    ClassDB::bind_method(D_METHOD("init", "callback"), &DECtalkMini::init);
    ClassDB::bind_method(D_METHOD("start", "text", "output_format"), &DECtalkMini::start);
    ClassDB::bind_method(D_METHOD("reset"), &DECtalkMini::reset);
    ClassDB::bind_method(D_METHOD("sync"), &DECtalkMini::sync);

    ADD_SIGNAL(MethodInfo("audio_chunk",
        PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data"),
        PropertyInfo(Variant::INT, "samples"),
        PropertyInfo(Variant::INT, "last_phoneme")
    ));

    BIND_CONSTANT(WAVE_FORMAT_1M16);
    BIND_CONSTANT(WAVE_FORMAT_08M16);
}

int DECtalkMini::init(const Callable &p_callback) {
    if (!p_callback.is_valid()) {
        ERR_PRINT("DECtalkMini.init: callback is invalid.");
        return -1;
    }

    return init_internal(&p_callback);
}

int DECtalkMini::init_internal(const Callable *p_callback) {
    std::lock_guard<std::mutex> lock(tts_mutex);

    if (active_instance != nullptr && active_instance != this) {
        ERR_PRINT("DECtalkMini.init: only one active instance is supported.");
        return -2;
    }

    if (p_callback != nullptr) {
        callback = *p_callback;
    }
    active_instance = this;

    if (initialized) {
        return 0;
    }

    int result = TextToSpeechInit(&DECtalkMini::tts_callback, nullptr);
    // Some builds return non-standard codes; mark initialized once init is called.
    initialized = true;

    return result;
}

int DECtalkMini::start(const String &p_text, int p_output_format) {
    std::lock_guard<std::mutex> lock(tts_mutex);

    if (!initialized) {
        ERR_PRINT("DECtalkMini.start: not initialized.");
        return -1;
    }

    tts_speaking.store(true);
    CharString utf8 = p_text.utf8();
    char *text_ptr = utf8.ptrw();

    int result = TextToSpeechStart(text_ptr, nullptr, p_output_format);
    if (result != ERR_NOERROR) {
        tts_speaking.store(false);
    }
    return result;
}

int DECtalkMini::reset() {
    std::lock_guard<std::mutex> lock(tts_mutex);

    int result = TextToSpeechReset();
    tts_speaking.store(false);
    return result;
}

int DECtalkMini::sync() {
    std::lock_guard<std::mutex> lock(tts_mutex);
    return TextToSpeechSync();
}

short *DECtalkMini::tts_callback(short *buffer, long length, int last_phoneme) {
    if (active_instance == nullptr) {
        return buffer;
    }
    return active_instance->on_callback(buffer, length, last_phoneme);
}

short *DECtalkMini::on_callback(short *buffer, long length, int last_phoneme) {
    // Mark progress; DECtalk Mini uses fixed 71-sample chunks,
    // and the final chunk is not divisible by 71.
    if (length > 0) {
        int64_t now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             std::chrono::steady_clock::now().time_since_epoch())
                             .count();
        last_audio_ns.store(now_ns);
        if ((length % 71) != 0) {
            tts_speaking.store(false);
        }
    }

    if (!callback.is_valid()) {
        return buffer;
    }

    PackedByteArray data;
    if (length > 0) {
        const int byte_count = static_cast<int>(length * static_cast<long>(sizeof(short)));
        data.resize(byte_count);
        memcpy(data.ptrw(), buffer, byte_count);
    }

    Array args;
    args.resize(3);
    args[0] = data;
    args[1] = static_cast<int64_t>(length);
    args[2] = static_cast<int64_t>(last_phoneme);
    callback.callv(args);
    emit_signal("audio_chunk", data, static_cast<int64_t>(length), static_cast<int64_t>(last_phoneme));

    return buffer;
}

} // namespace godot
