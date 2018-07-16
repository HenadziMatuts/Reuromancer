#include "neuro_routines.h"

int asm_set_track_on_playback(int track_num);
int asm_get_sample();

static const float g_timer_divisor = 0x13b1;
static const float g_timer_clock_hz = 1193180;
static const float g_sample_rate_hz = 44100;

int build_track_waveform(int track_num, uint8_t *waveform, int len)
{
	float timer_freq_hz = g_timer_clock_hz / g_timer_divisor;
	float timer_period_sec = 1 / timer_freq_hz;
	float sample_period_sec = 1 / g_sample_rate_hz;
	float passed = 0;
	int sample = 0;
	int silence_counter = 0;
	uint8_t sample_val = 128, sample_want = 0, sample_step = 32;

	asm_set_track_on_playback(track_num);

	while (sample != len)
	{
		int divisor = asm_get_sample() & 0xffff;
		float freq_hz = g_timer_clock_hz / divisor;
		float half_period_sec = (1 / freq_hz) / 2;
		float temp_half_period = half_period_sec;

		if (divisor)
		{
			if (half_period_sec >= timer_period_sec ||
				half_period_sec >= passed)
			{
				half_period_sec -= passed;
			}
			silence_counter = 0;
		}
		else if (++silence_counter == 250)
		{
			break; // a second of silence, time to stop
		}
		passed = 0;

		float till_next_call_sec = timer_period_sec;
		while (passed <= till_next_call_sec)
		{
			if (divisor)
			{
				if (passed + sample_period_sec > half_period_sec)
				{
					sample_want = (sample_val == 128) ? 64 : (-1 * sample_val);
					till_next_call_sec -= passed;
					half_period_sec = temp_half_period;
					passed = 0;
				}
			}
			else
			{
				sample_want = 128;
			}

			if (sample_want)
			{
				if (sample_want < sample_val)
				{
					sample_val -= sample_step;
				}
				else if (sample_want > sample_val)
				{
					sample_val += sample_step;
				}
				else if (sample_want == sample_val)
				{
					sample_want = 0;
				}
			}

			if (sample == len)
			{
				break;
			}

			passed += sample_period_sec;
			waveform[sample++] = sample_val;
		}
	}

	return sample;
}
