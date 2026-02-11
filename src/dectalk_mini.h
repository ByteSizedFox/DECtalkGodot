#ifndef DECTALK_MINI_H
#define DECTALK_MINI_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <atomic>
#include <cstdint>
#include <mutex>

namespace godot {

class DECtalkMini : public RefCounted {
    GDCLASS(DECtalkMini, RefCounted)

public:
    DECtalkMini();
    ~DECtalkMini();

    int init(const Callable &p_callback);
    int start(const String &p_text, int p_output_format);
    int reset();
    int sync();
    static short *tts_callback(short *buffer, long length, int last_phoneme);

protected:
    static void _bind_methods();

private:
    int init_internal(const Callable *p_callback);

    short *on_callback(short *buffer, long length, int last_phoneme);

    Callable callback;
    bool initialized = false;

    mutable std::mutex tts_mutex;
    std::atomic<bool> tts_speaking{false};
    std::atomic<int64_t> last_audio_ns{0};

    static DECtalkMini *active_instance;
};

} // namespace godot

#endif // DECTALK_MINI_H
