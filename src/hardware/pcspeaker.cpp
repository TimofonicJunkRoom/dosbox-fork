/*
 *  Copyright (C) 2002  The DOSBox Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <dosbox.h>
#include <mixer.h>
#include <math.h>


#ifndef PI
#define PI 3.14159265358979323846
#endif

#define SPKR_RATE 22050
#define SPKR_VOLUME 5000

#define FREQ_SHIFT 16
#define FREQ_MAX (2 << FREQ_SHIFT)
#define FREQ_HALF (FREQ_MAX >> 1)



struct Speaker {
	Bit32u freq_add;
	Bit32u freq_pos;
	Bit16s volume;
	MIXER_Channel * chan;
	bool enabled;
};


static Speaker spkr;


void PCSPEAKER_SetFreq(Bit32u freq) {
	spkr.freq_add=(Bit32u)(FREQ_MAX/((float)SPKR_RATE/(float)freq));
	spkr.freq_pos=0;
}

void PCSPEAKER_Enable(bool enable) {
	spkr.enabled=enable;
	MIXER_Enable(spkr.chan,enable);
}

static void PCSPEAKER_CallBack(Bit8u * stream,Bit32u len) {
	/* Generate the speaker wave, TODO Improve :) */
	for (Bit32u c=0;c<len;c++) {
		spkr.freq_pos+=spkr.freq_add;
		if (spkr.freq_pos>=FREQ_MAX) spkr.freq_pos-=FREQ_MAX;
		if (spkr.freq_pos>=FREQ_HALF) {
			*(Bit16s*)(stream)=spkr.volume;
		} else {
			*(Bit16s*)(stream)=-spkr.volume;
		}
/*
		if (spkr.freq_pos>=FREQ_HALF) { 
			spkr.freq_pos-=FREQ_HALF;
			spkr.volume=-spkr.volume;
		};
		*(Bit16s*)(stream)=spkr.volume;
*/
		stream+=2;
	}

}

void PCSPEAKER_Init(void) {
	spkr.chan=MIXER_AddChannel(&PCSPEAKER_CallBack,SPKR_RATE,"PC-SPEAKER");
	MIXER_Enable(spkr.chan,false);
	MIXER_SetMode(spkr.chan,MIXER_16MONO);
	spkr.volume=SPKR_VOLUME;
	spkr.enabled=false;
}
