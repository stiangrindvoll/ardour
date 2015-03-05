/*
 * Copyright (C) 2015 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>

#include <map>
#include <string>

#define AUHAL_OUTPUT_ELEMENT 0
#define AUHAL_INPUT_ELEMENT 1


class CoreAudioPCM {
public:
	CoreAudioPCM (void);
	~CoreAudioPCM (void);


	int      state (void) const { return _state; }
	uint32_t n_playback_channels (void) const { return _playback_channels; }
	uint32_t n_capture_channels (void) const { return _capture_channels; }

	void     discover();
	void     device_list(std::map<size_t, std::string> &devices) const { devices = _devices;}

	void     pcm_stop (void);
	int      pcm_start (
			uint32_t input_device,
			uint32_t output_device,
			uint32_t sample_rate,
			uint32_t samples_per_period,
			int (process_callback (void*)),
			void * process_arg
			);

	void     set_error_callback (
			void ( error_callback (void*)),
			void * error_arg
                        ) {
                _error_callback = error_callback;
                _error_arg = error_arg;
        }

	// must be called from process_callback;
	int      get_capture_channel (uint32_t chn, float *input, uint32_t n_samples);
	int      set_playback_channel (uint32_t chn, const float *input, uint32_t n_samples);
	uint32_t n_samples() const { return _cur_samples_per_period; };

	// really private
	OSStatus render_callback (
			AudioUnitRenderActionFlags* ioActionFlags,
			const AudioTimeStamp* inTimeStamp,
			UInt32 inBusNumber,
			UInt32 inNumberFrames,
			AudioBufferList* ioData);

        void hwPropertyChange();

private:
	AudioUnit _auhal;
	AudioDeviceID* _deviceIDs;
	AudioBufferList* _inputAudioBufferList;
	AudioBufferList* _outputAudioBufferList;

	int _state;

	uint32_t _max_samples_per_period;
	uint32_t _cur_samples_per_period;
	uint32_t _capture_channels;
	uint32_t _playback_channels;
	bool     _in_process;
	size_t   _numDevices;

	int (* _process_callback) (void*);
	void * _process_arg;

	void (* _error_callback) (void*);
	void  * _error_arg;

	std::map<size_t, std::string> _devices;
        // TODO proper device info struct
	uint32_t * _device_ins;
	uint32_t * _device_outs;

};
