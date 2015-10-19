/*
 * Straightforward direct form 1 discrete filter implementation
 *
 * (c) 2013 Benedikt Hofmeister
 */

/*
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA or point your web browser to http://www.gnu.org.
*/


#include "DirectFormI.h"

//#define SPAM_OUTPUT

#ifdef SPAM_OUTPUT
	#include <stdio.h>
#endif


void
directFormI(float* pIn, float* pOut, const uint nSamples,
		float* bs, float* xs, const uint nBs,
		float* as, float* ys, const uint nAs,
		int* pXPos, int* pYPos)
{
	const float fOverA0 = 1.0f / as[0];
	uint uiSample = 0;
	for(; uiSample < nSamples; uiSample++)
	{
		xs[*pXPos] = pIn[uiSample];

		int i = 0;

#ifdef SPAM_OUTPUT
		printf("sample: %f\nbs:[", pIn[uiSample]);

		for(; i < nBs ; i++)
			printf("%f ", bs[i]);

		printf("]\nas:[");

		i = 0;
		for(; i < nAs ; i++)
			printf("%f ", as[i]);

		printf("]\nxs:[");
#endif
		float fOut = 0;

		i = 0;
		for(; i < nBs ; i++)
		{
			int xPos = *pXPos - i;
			if(xPos < 0)
				xPos += (int)nBs;
			fOut += bs[i] * xs[xPos];
#ifdef SPAM_OUTPUT
			printf("%f ", xs[xPos]);
#endif
		}

#ifdef SPAM_OUTPUT
		printf("], b_i*x_i: %f\nys:[", fOut);
#endif

		int j = 1;
		for(; j < nAs ; j++)
		{
			int yPos = *pYPos - j;
			if(yPos < 0)
				yPos += (int)nAs;
			fOut -= as[j] * ys[yPos];

#ifdef SPAM_OUTPUT
			printf("%f ", ys[yPos]);
#endif
		}

		pOut[uiSample] = CLAMP( (ys[*pYPos] = fOut * fOverA0), -1.0f, 1.0f);

#ifdef SPAM_OUTPUT
		printf("], output: %f\n", pOut[uiSample]);
#endif

		if(++(*pXPos) >= nBs)
			*pXPos = 0;

		if(++(*pYPos) >= nAs)
			*pYPos = 0;
	}
}
