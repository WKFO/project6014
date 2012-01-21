//Copyright Paul Reiche, Fred Ford. 1992-2002

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

// JMS 2009: -Orz space starmap colors and star locations
// JMS 2010: -Do not draw Sphere of Influence for Slylandros riding Kohr-Ah ships
//			 -Do not draw Sphere of Influence for Kohr-Ahs.
//			 -Display hint for the player to use the star search facility in the starmap
// JMS 2011: -In the beginning of game, known races' home planets are marked on the map as small SoIs.
//			 -The hi-res modes now have a more user-friendly starmap cursor.

// JMS_GFX 2011: Merged the resolution Factor stuff from UQM-HD.

#include "scan.h"
#include "triangul.h"
#include "../colors.h"
#include "../controls.h"
#include "../menustat.h"
// XXX: for stuff that does not belong there
#include "../encount.h"
#include "../races.h"
#include "../gameopt.h"
#include "../gamestr.h"
#include "../globdata.h"
#include "../shipcont.h"
#include "../units.h"
#include "../hyper.h" // for SOL_X/SOL_Y
#include "../sis.h"
		// for DrawHyperCoords(), DrawStatusMessage()
#include "../settings.h"
#include "../setup.h"
#include "../sounds.h"
#include "../state.h"
#include "../uqmdebug.h"
#include "options.h"
#include "libs/inplib.h"
#include "libs/strlib.h"
#include "libs/graphics/gfx_common.h"
#include "libs/mathlib.h"
#include "libs/memlib.h"

#include "libs/log.h"
#include "../nameref.h"

#include <stdlib.h>
#include <math.h>


static POINT cursorLoc;
static POINT mapOrigin;
static int zoomLevel;
static FRAME StarMapFrame;


static inline long
signedDivWithError (long val, long divisor)
{
	int invert = 0;
	if (val < 0)
	{
		invert = 1;
		val = -val;
	}
	val = (val + ROUNDING_ERROR (divisor)) / divisor;
	return invert ? -val : val;
}

#define MAP_FIT_X ((MAX_X_UNIVERSE + 1) / SIS_SCREEN_WIDTH + 1)

static inline COORD
universeToDispx (COORD ux)
{
	return signedDivWithError ((((long)ux - mapOrigin.x) << zoomLevel)
			* SIS_SCREEN_WIDTH, MAX_X_UNIVERSE + MAP_FIT_X)
			+ ((SIS_SCREEN_WIDTH - 1) >> 1);
}
#define UNIVERSE_TO_DISPX(ux)  universeToDispx(ux)

static inline COORD
universeToDispy (COORD uy)
{
	return signedDivWithError ((((long)mapOrigin.y - uy) << zoomLevel)
			* SIS_SCREEN_HEIGHT, MAX_Y_UNIVERSE + 2)
			+ ((SIS_SCREEN_HEIGHT - 1) >> 1);
}
#define UNIVERSE_TO_DISPY(uy)  universeToDispy(uy)

static inline COORD
dispxToUniverse (COORD dx)
{
	return (((long)(dx - ((SIS_SCREEN_WIDTH - 1) >> 1))
			* (MAX_X_UNIVERSE + MAP_FIT_X)) >> zoomLevel)
			/ SIS_SCREEN_WIDTH + mapOrigin.x;
}
#define DISP_TO_UNIVERSEX(dx)  dispxToUniverse(dx)

static inline COORD
dispyToUniverse (COORD dy)
{
	return (((long)(((SIS_SCREEN_HEIGHT - 1) >> 1) - dy)
			* (MAX_Y_UNIVERSE + 2)) >> zoomLevel)
			/ SIS_SCREEN_HEIGHT + mapOrigin.y;
}
#define DISP_TO_UNIVERSEY(dy)  dispyToUniverse(dy)

static BOOLEAN transition_pending;

static void
flashCurrentLocation (POINT *where)
{
	static BYTE c = 0;
	static int val = -2;
	static POINT universe;
	static TimeCount NextTime = 0;

	if (where)
		universe = *where;

	if (GetTimeCounter () >= NextTime)
	{
		Color OldColor;
		CONTEXT OldContext;
		STAMP s;

		NextTime = GetTimeCounter () + (ONE_SECOND / 16);
		
		LockMutex (GraphicsLock);
		OldContext = SetContext (SpaceContext);

		if (c == 0x00 || c == 0x1A)
			val = -val;
		c += val;
		OldColor = SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (c, c, c), c));
		s.origin.x = UNIVERSE_TO_DISPX (universe.x);
		s.origin.y = UNIVERSE_TO_DISPY (universe.y);
		s.frame = IncFrameIndex (StarMapFrame);
		DrawFilledStamp (&s);
		SetContextForeGroundColor (OldColor);

		SetContext (OldContext);
		UnlockMutex (GraphicsLock);
	}
}

static void
DrawCursor (COORD curs_x, COORD curs_y)
{
	STAMP s;

	s.origin.x = curs_x;
	s.origin.y = curs_y;
	s.frame = StarMapFrame;

	DrawStamp (&s);
}

static void DrawAutoPilotSegment(POINT *pt, POINT *destPt)
{
	SIZE dx, dy,
				xincr, yincr,
				xerror, yerror,
				cycle, delta;

	dx = destPt->x - pt->x;
	if (dx >= 0)
		xincr = 1;
	else
	{
		xincr = -1;
		dx = -dx;
	}
	dx <<= 1;

	dy = destPt->y - pt->y;
	if (dy >= 0)
		yincr = 1;
	else
	{
		yincr = -1;
		dy = -dy;
	}
	dy <<= 1;

	if (dx >= dy)
		cycle = dx;
	else
		cycle = dy;
	delta = xerror = yerror = cycle >> 1;

	delta &= ~1;
	while (delta--)
	{
		if (!(delta & 1))
			DrawPoint (pt);

		if ((xerror -= dx) <= 0)
		{
			pt->x += xincr;
			xerror += cycle;
		}
		if ((yerror -= dy) <= 0)
		{
			pt->y += yincr;
			yerror += cycle;
		}
	}
}

static void
DrawAutoPilot (POINT *pDstPt)
{
	SIZE dx, dy,
				xincr, yincr,
				xerror, yerror,
				cycle, delta;
	POINT pt;
    POINT destPt;

	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
		pt = CurStarDescPtr->star_pt;
	else
	{
		pt.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
		pt.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
	}
	pt.x = UNIVERSE_TO_DISPX (pt.x);
	pt.y = UNIVERSE_TO_DISPY (pt.y);

    destPt.x = UNIVERSE_TO_DISPX (pDstPt->x);
    destPt.y = UNIVERSE_TO_DISPY (pDstPt->y);

	SetContextForeGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x04, 0x04, 0x1F), 0x01));

    DrawAutoPilotSegment(&pt, &destPt);
}

void SetOrzSpaceAutoPilot(void)
{
    float px = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
    float py = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
    float qx = GLOBAL (autopilot).x;
    float qy = GLOBAL (autopilot).y;

    float dx = qx - px;
    float dy = qy - py;

    float tx = (px + qx)/2.0;
    float ty = (py + qy)/2.0;

    float k = ty/dx;
    float cx = tx + k*dy;

    float r2 = qy*qy + (qx-cx)*(qx-cx);
    float rad = sqrt(r2);

    GLOBAL (autopilotOrzSpaceCenter).x = cx;
    GLOBAL (autopilotOrzSpaceCenter).y = 0;

    GLOBAL (autopilotOrzSpaceRadius) = rad;
}

static void
DrawOrzSpaceAutoPilot (POINT *pDstPt)
{
    SDWORD cx = GLOBAL (autopilotOrzSpaceCenter).x;
    SDWORD cy = GLOBAL (autopilotOrzSpaceCenter).y;
    SDWORD rad = GLOBAL (autopilotOrzSpaceRadius);

    SDWORD x1 = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
    SDWORD y1 = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
    SDWORD x2 = GLOBAL (autopilot).x;
    SDWORD y2 = GLOBAL (autopilot).y;

    float a1, a2;
    float deltaA;
    int numSegs;
    int i;

    POINT pt, destPt;

    a1 = atan2(y1 - cy, x1 - cx);
    a2 = atan2(y2 - cy, x2 - cx);

    numSegs = abs(32.0*(a2 - a1)/M_PI);
    if (numSegs < 1)
        numSegs = 1;

    deltaA = (a2 - a1)/numSegs;

	SetContextForeGroundColor (
			BUILD_COLOR (MAKE_RGB15 (0x04, 0x04, 0x1F), 0x01));

    pt.x = UNIVERSE_TO_DISPX (x1);
    pt.y = UNIVERSE_TO_DISPY (y1);

    for (i = 0; i < numSegs; i++)
    {
        a1 += deltaA;
        destPt.x = UNIVERSE_TO_DISPX (cx + rad * cos(a1));
        destPt.y = UNIVERSE_TO_DISPY (cy + rad * sin(a1));

        DrawAutoPilotSegment(&pt, &destPt);
    }
}

static void
GetSphereRect (FLEET_INFO *FleetPtr, RECT *pRect, RECT *pRepairRect)
{
	long diameter;
	
	// JMS: Negative known strength marks initial state of the SoI's: 
	// Only the home planet of certain races is known -> show small sphere.
	if (FleetPtr->known_strength >= 0)
		diameter = (long)(FleetPtr->known_strength * SPHERE_RADIUS_INCREMENT);
	else
		diameter = (long)(-(FleetPtr->known_strength) * SPHERE_RADIUS_INCREMENT);
		
	pRect->extent.width = UNIVERSE_TO_DISPX (diameter) - UNIVERSE_TO_DISPX (0);
	
	if (pRect->extent.width < 0)
		pRect->extent.width = -pRect->extent.width;
	else if (pRect->extent.width == 0)
		pRect->extent.width = 1;
	
	pRect->extent.height = UNIVERSE_TO_DISPY (diameter)- UNIVERSE_TO_DISPY (0);
	
	if (pRect->extent.height < 0)
		pRect->extent.height = -pRect->extent.height;
	else if (pRect->extent.height == 0)
		pRect->extent.height = 1;

	pRect->corner.x = UNIVERSE_TO_DISPX (FleetPtr->known_loc.x);
	pRect->corner.y = UNIVERSE_TO_DISPY (FleetPtr->known_loc.y);
	pRect->corner.x -= pRect->extent.width >> 1;
	pRect->corner.y -= pRect->extent.height >> 1;

	{
		TEXT t;
		STRING locString;

		SetContextFont (TinyFont);

		t.baseline.x = pRect->corner.x + (pRect->extent.width >> 1);
		t.baseline.y = pRect->corner.y + (pRect->extent.height >> 1) - 1;
		t.align = ALIGN_CENTER;
		locString = SetAbsStringTableIndex (FleetPtr->race_strings, 1);
		t.CharCount = GetStringLength (locString);
		t.pStr = (UNICODE *)GetStringAddress (locString);
		TextRect (&t, pRepairRect, NULL);
		
		if (pRepairRect->corner.x <= 0)
			pRepairRect->corner.x = 1;
		else if (pRepairRect->corner.x + pRepairRect->extent.width >=
				SIS_SCREEN_WIDTH)
			pRepairRect->corner.x =
					SIS_SCREEN_WIDTH - pRepairRect->extent.width - 1;
		if (pRepairRect->corner.y <= 0)
			pRepairRect->corner.y = 1;
		else if (pRepairRect->corner.y + pRepairRect->extent.height >=
				SIS_SCREEN_HEIGHT)
			pRepairRect->corner.y =
					SIS_SCREEN_HEIGHT - pRepairRect->extent.height - 1;

		BoxUnion (pRepairRect, pRect, pRepairRect);
		pRepairRect->extent.width++;
		pRepairRect->extent.height++;
	}
}

static void
DrawStarMap (COUNT race_update, RECT *pClipRect)
{
#define GRID_DELTA 500
	SIZE i, j;
	COUNT which_space;
	COUNT orz_space;	// JMS
	long diameter;
	RECT r, old_r;
	POINT oldOrigin = {0, 0};
	STAMP s, nebula;
	FRAME star_frame;
	STAR_DESC *SDPtr;
	BOOLEAN draw_cursor;
	Color OldColor; // JMS: Relocated here from the fuel range drawing loop, needed for transparent grid.

	if (pClipRect == (RECT*)-1)
	{
		pClipRect = 0;
		draw_cursor = FALSE;
	}
	else
	{
		LockMutex (GraphicsLock);
		draw_cursor = TRUE;
	}

	SetContext (SpaceContext);
	if (pClipRect)
	{
		GetContextClipRect (&old_r);
		pClipRect->corner.x += old_r.corner.x;
		pClipRect->corner.y += old_r.corner.y;
		SetContextClipRect (pClipRect);
		pClipRect->corner.x -= old_r.corner.x;
		pClipRect->corner.y -= old_r.corner.y;
		// Offset the origin so that we draw the correct gfx in the cliprect
		oldOrigin = SetContextOrigin (MAKE_POINT (-pClipRect->corner.x,
				-pClipRect->corner.y));
	}

	if (transition_pending)
	{
		SetTransitionSource (NULL);
	}
	BatchGraphics ();
	
	which_space = GET_GAME_STATE (ARILOU_SPACE_SIDE);
	orz_space= GET_GAME_STATE (ORZ_SPACE_SIDE);	// JMS

	if (which_space <= 1)
	{
		SDPtr = &star_array[0];
		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x07), 0x57));
		SetContextBackGroundColor (BLACK_COLOR);
		
	}
	else
	{
		SDPtr = &star_array[NUM_SOLAR_SYSTEMS + 1];
		SetContextForeGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x00, 0x0B, 0x00), 0x6D));
		SetContextBackGroundColor (
				BUILD_COLOR (MAKE_RGB15 (0x00, 0x08, 0x00), 0x6E));
	}
	if(orz_space > 1)// JMS: ORZ space starmap BG color
	{
		SDPtr = &star_array[NUM_SOLAR_SYSTEMS + 15 + 2 + 1]; // JMS: ORZ space starsystems, 15 = number of 
		SetContextForeGroundColor (						     // quasispace vortices, 2 = Two space size definitions, 1 = arilou world  
								   BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x0B), 0x6D));
		SetContextBackGroundColor (
								   BUILD_COLOR (MAKE_RGB15 (0x00, 0x00, 0x08), 0x6E));
	}
	ClearDrawable ();
	
	// JMS: Draw a sexy nebula on the map's background (Only in Hyperspace, not in QS/Orzspace!).
	if (which_space <= 1)
	{
		nebula.origin.x = nebula.origin.y = 0;
		nebula.frame = SetAbsFrameIndex (nebulaeFrame, 16);
		DrawStamp(&nebula);
	}
	
	if (race_update == 0
			&& which_space < 2
			&& orz_space < 2 // JMS: Orz space check
			&& (diameter = (long)GLOBAL_SIS (FuelOnBoard) << 1))
	{
		if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
			r.corner = CurStarDescPtr->star_pt;
		else
		{
			r.corner.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
			r.corner.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
		}

		r.extent.width = UNIVERSE_TO_DISPX (diameter)
				- UNIVERSE_TO_DISPX (0);
		if (r.extent.width < 0)
			r.extent.width = -r.extent.width;
		r.extent.height = UNIVERSE_TO_DISPY (diameter)
				- UNIVERSE_TO_DISPY (0);
		if (r.extent.height < 0)
			r.extent.height = -r.extent.height;

		r.corner.x = UNIVERSE_TO_DISPX (r.corner.x)
				- (r.extent.width >> 1);
		r.corner.y = UNIVERSE_TO_DISPY (r.corner.y)
				- (r.extent.height >> 1);

		// JMS: This was the original fuel range circle color.
		//OldColor = SetContextForeGroundColor (
		//		BUILD_COLOR (MAKE_RGB15 (0x03, 0x03, 0x03), 0x22));
		
		// JMS: The fuel range circle is now partially Transparent
		OldColor = SetContextForeGroundColor (BUILD_COLOR_RGBA(0x66,0x66,0x66,0x22));
		
		DrawFilledOval (&r);
		
		SetContextForeGroundColor (OldColor);
	}
	
	// JMS: The grid is now partially transparent for HS map.
	// Hi-res modes have less transparency since thin lines are harder to see.
	// JvdD: The grid is now bright red, to account for the background 
	if (which_space <= 1)
		OldColor = SetContextForeGroundColor (BUILD_COLOR_RGBA(0xAE,0x30,0x30,RES_CASE(0x55,0x80,0xCC)));
	
	// The Grid. (For more information, see TRON)
	for (i = MAX_Y_UNIVERSE + 1, j = MAX_X_UNIVERSE + 1; i >= 0; i -= GRID_DELTA, j -= GRID_DELTA)
	{
		// Horizontal lines.
		r.corner.x = UNIVERSE_TO_DISPX (0);
		r.corner.y = UNIVERSE_TO_DISPY (i);
		r.extent.width = SIS_SCREEN_WIDTH << zoomLevel;
		r.extent.height = 1;
		DrawFilledRectangle (&r);

		// Vertical lines
		r.corner.y = UNIVERSE_TO_DISPY (MAX_Y_UNIVERSE + 1);
		r.extent.width = 1;
		r.extent.height = SIS_SCREEN_HEIGHT << zoomLevel;
		r.corner.x = UNIVERSE_TO_DISPX (j);
		DrawFilledRectangle (&r);
	}
	
	// JMS: Return original nontransparent grid color, just in case.
	if (which_space <= 1)
		SetContextForeGroundColor (OldColor);

	star_frame = SetRelFrameIndex (StarMapFrame, 2);
	
	// Draw Spheres of Influence
	if (which_space <= 1 && orz_space <= 1) // JMS: Orz space check
	{
		COUNT index;
		HFLEETINFO hStarShip, hNextShip;
		static const Color race_colors[] =
		{
			RACE_COLORS
		};

		for (index = 0,
				hStarShip = GetHeadLink (&GLOBAL (avail_race_q));
				hStarShip != 0; ++index, hStarShip = hNextShip)
		{
			FLEET_INFO *FleetPtr;

			FleetPtr = LockFleetInfo (&GLOBAL (avail_race_q), hStarShip);
			hNextShip = _GetSuccLink (FleetPtr);

			if (FleetPtr->known_strength 
				&& FleetPtr->SpeciesID!=SLYLANDRO_KOHRAH_ID // JMS: Don't draw circle for Slylandro-Kohrahs
				&& FleetPtr->SpeciesID!=KOHR_AH_ID)			// JMS: Don't draw circle for Kohr-Ahs
			{
				RECT repair_r;

				GetSphereRect (FleetPtr, &r, &repair_r);
				if (r.corner.x < SIS_SCREEN_WIDTH
						&& r.corner.y < SIS_SCREEN_HEIGHT
						&& r.corner.x + r.extent.width > 0
						&& r.corner.y + r.extent.height > 0
						&& (pClipRect == 0
						|| (repair_r.corner.x < pClipRect->corner.x + pClipRect->extent.width
						&& repair_r.corner.y < pClipRect->corner.y + pClipRect->extent.height
						&& repair_r.corner.x + repair_r.extent.width > pClipRect->corner.x
						&& repair_r.corner.y + repair_r.extent.height > pClipRect->corner.y)))
				{
					Color c;
					TEXT t;
					STRING locString;

					c = race_colors[index];
					if (index + 1 == race_update)
						SetContextForeGroundColor (WHITE_COLOR);
					else
						SetContextForeGroundColor (c);
					DrawOval (&r, 0, 0);

					SetContextFont (TinyFont);

					t.baseline.x = r.corner.x + (r.extent.width >> 1);
					t.baseline.y = r.corner.y + (r.extent.height >> 1) - 1;
					t.align = ALIGN_CENTER;
					locString = SetAbsStringTableIndex (
							FleetPtr->race_strings, 1);
					t.CharCount = GetStringLength (locString);
					t.pStr = (UNICODE *)GetStringAddress (locString);
					TextRect (&t, &r, NULL);

					if (r.corner.x <= 0)
						t.baseline.x -= r.corner.x - 1;
					else if (r.corner.x + r.extent.width >= SIS_SCREEN_WIDTH)
						t.baseline.x -= (r.corner.x + r.extent.width)
								- SIS_SCREEN_WIDTH + 1;
					if (r.corner.y <= 0)
						t.baseline.y -= r.corner.y - 1;
					else if (r.corner.y + r.extent.height >= SIS_SCREEN_HEIGHT)
						t.baseline.y -= (r.corner.y + r.extent.height)
								- SIS_SCREEN_HEIGHT + 1;

					// The text color is slightly lighter than the color of
					// the SoI.
					c.r = (c.r >= 0xff - CC5TO8 (0x03)) ?
							0xff : c.r + CC5TO8 (0x03);
					c.g = (c.g >= 0xff - CC5TO8 (0x03)) ?
							0xff : c.g + CC5TO8 (0x03);
					c.b = (c.b >= 0xff - CC5TO8 (0x03)) ?
							0xff : c.b + CC5TO8 (0x03);

					SetContextForeGroundColor (c);
					font_DrawText (&t);
				}
			}

			UnlockFleetInfo (&GLOBAL (avail_race_q), hStarShip);
		}
	}

	// JMS: Shofixti crash site triangulation sphere drawing, defined in planets/triangul.c
	//if(!(GET_GAME_STATE(HIDE_TRIANGULATION_SPHERES)))
	if(1)
		drawTriangulationSpheres(which_space, orz_space, pClipRect, mapOrigin, zoomLevel);

	do
	{
		BYTE star_type;
		star_type = SDPtr->Type;

		if (SDPtr->Index != MELNORME_HOME_DEFINED)
		{

			s.origin.x = UNIVERSE_TO_DISPX (SDPtr->star_pt.x);
			s.origin.y = UNIVERSE_TO_DISPY (SDPtr->star_pt.y);
			if (which_space <= 1)
				s.frame = SetRelFrameIndex (star_frame,
						STAR_TYPE (star_type)
						* NUM_STAR_COLORS
						+ STAR_COLOR (star_type));
			else if (SDPtr->star_pt.x == ARILOU_HOME_X
					&& SDPtr->star_pt.y == ARILOU_HOME_Y)
				s.frame = SetRelFrameIndex (star_frame,
						SUPER_GIANT_STAR * NUM_STAR_COLORS + GREEN_BODY);
			else
				s.frame = SetRelFrameIndex (star_frame,
						GIANT_STAR * NUM_STAR_COLORS + GREEN_BODY);
			DrawStamp (&s);
		}

		++SDPtr;
	} while (SDPtr->star_pt.x <= MAX_X_UNIVERSE && SDPtr->star_pt.y <= MAX_Y_UNIVERSE);

	if (GET_GAME_STATE (ARILOU_SPACE))
	{
		if (which_space <= 1)
		{
			s.origin.x = UNIVERSE_TO_DISPX (ARILOU_SPACE_X);
			s.origin.y = UNIVERSE_TO_DISPY (ARILOU_SPACE_Y);
		}
		else
		{
			s.origin.x = UNIVERSE_TO_DISPX (QUASI_SPACE_X);
			s.origin.y = UNIVERSE_TO_DISPY (QUASI_SPACE_Y);
		}
		s.frame = SetRelFrameIndex (star_frame, GIANT_STAR * NUM_STAR_COLORS + GREEN_BODY);
		DrawStamp (&s);
	}

	if (race_update == 0
			&& GLOBAL (autopilot.x) != ~0
			&& GLOBAL (autopilot.y) != ~0)
    {
        if (GET_GAME_STATE (ORZ_SPACE_SIDE) > 1)
		    DrawOrzSpaceAutoPilot (&GLOBAL (autopilot));
        else
            DrawAutoPilot (&GLOBAL (autopilot));
    }

	if (transition_pending)
	{
		GetContextClipRect (&r);
		ScreenTransition (3, &r);
		transition_pending = FALSE;
	}
	UnbatchGraphics ();

	if (pClipRect)
	{
		SetContextClipRect (&old_r);
		SetContextOrigin (oldOrigin);
	}

	if (race_update == 0)
	{
		if (draw_cursor)
		{
			GetContextClipRect (&r);
			LoadIntoExtraScreen (&r);
			DrawCursor (UNIVERSE_TO_DISPX (cursorLoc.x),
					UNIVERSE_TO_DISPY (cursorLoc.y));
		}
	}
	if (draw_cursor)
		UnlockMutex (GraphicsLock);
}

static void
EraseCursor (COORD curs_x, COORD curs_y)
{
	RECT r;

	GetFrameRect (StarMapFrame, &r);

	if ((r.corner.x += curs_x) < 0)
	{
		r.extent.width += r.corner.x;
		r.corner.x = 0;
	}
	else if (r.corner.x + r.extent.width >= SIS_SCREEN_WIDTH)
		r.extent.width = SIS_SCREEN_WIDTH - r.corner.x;
	if ((r.corner.y += curs_y) < 0)
	{
		r.extent.height += r.corner.y;
		r.corner.y = 0;
	}
	else if (r.corner.y + r.extent.height >= SIS_SCREEN_HEIGHT)
		r.extent.height = SIS_SCREEN_HEIGHT - r.corner.y;

#ifndef OLD
	RepairBackRect (&r, FALSE);
#else /* NEW */
	r.extent.height += r.corner.y & 1;
	r.corner.y &= ~1;
	UnlockMutex (GraphicsLock);
	DrawStarMap (0, &r);
	LockMutex (GraphicsLock);
#endif /* OLD */
}

static void
ZoomStarMap (SIZE dir)
{
#define MAX_ZOOM_SHIFT (BYTE)(4 - RESOLUTION_FACTOR)
	if (dir > 0)
	{
		if (zoomLevel < MAX_ZOOM_SHIFT)
		{
			++zoomLevel;
			mapOrigin = cursorLoc;

			DrawStarMap (0, NULL);
			SleepThread (ONE_SECOND / 8);
		}
	}
	else if (dir < 0)
	{
		if (zoomLevel > 0)
		{
			if (zoomLevel > 1)
				mapOrigin = cursorLoc;
			else
			{
				mapOrigin.x = MAX_X_UNIVERSE >> 1;
				mapOrigin.y = MAX_Y_UNIVERSE >> 1;
			}
			--zoomLevel;

			DrawStarMap (0, NULL);
			SleepThread (ONE_SECOND / 8);
		}
	}
}

static void
UpdateCursorLocation (int sx, int sy, const POINT *newpt)
{
	STAMP s;
	POINT pt;

	pt.x = UNIVERSE_TO_DISPX (cursorLoc.x);
	pt.y = UNIVERSE_TO_DISPY (cursorLoc.y);

	if (newpt)
	{	// absolute move
		sx = sy = 0;
		s.origin.x = UNIVERSE_TO_DISPX (newpt->x);
		s.origin.y = UNIVERSE_TO_DISPY (newpt->y);
		cursorLoc = *newpt;
	}
	else
	{	// incremental move
		s.origin.x = pt.x + sx;
		s.origin.y = pt.y + sy;
	}

	if (sx)
	{
		cursorLoc.x = DISP_TO_UNIVERSEX (s.origin.x) - sx;
		while (UNIVERSE_TO_DISPX (cursorLoc.x) == pt.x)
			cursorLoc.x += sx;
		
		if (cursorLoc.x < 0)
			cursorLoc.x = 0;
		else if (cursorLoc.x > MAX_X_UNIVERSE)
			cursorLoc.x = MAX_X_UNIVERSE;

		s.origin.x = UNIVERSE_TO_DISPX (cursorLoc.x);
	}

	if (sy)
	{
		cursorLoc.y = DISP_TO_UNIVERSEY (s.origin.y) + sy;
		while (UNIVERSE_TO_DISPY (cursorLoc.y) == pt.y)
			cursorLoc.y -= sy;

		if (cursorLoc.y < 0)
			cursorLoc.y = 0;
		else if (cursorLoc.y > MAX_Y_UNIVERSE)
			cursorLoc.y = MAX_Y_UNIVERSE;

		s.origin.y = UNIVERSE_TO_DISPY (cursorLoc.y);
		if (s.origin.y < 0)
		{
			s.origin.y = 0;
			cursorLoc.y = DISP_TO_UNIVERSEY (0);
		}
	}

	if (s.origin.x < 0 || s.origin.y < 0
			|| s.origin.x >= SIS_SCREEN_WIDTH
			|| s.origin.y >= SIS_SCREEN_HEIGHT)
	{
		mapOrigin = cursorLoc;
		DrawStarMap (0, NULL);

		s.origin.x = UNIVERSE_TO_DISPX (cursorLoc.x);
		s.origin.y = UNIVERSE_TO_DISPY (cursorLoc.y);
	}
	else
	{
		LockMutex (GraphicsLock);
		EraseCursor (pt.x, pt.y);
		// ClearDrawable ();
		DrawCursor (s.origin.x, s.origin.y);
		UnlockMutex (GraphicsLock);
	}
}

#define CURSOR_INFO_BUFSIZE 256

// JMS: How close to a star the cursor has to be to 'snap' into it.
// Don't make this larger than 1 for lo-res(1x). Otherwise the cursor gets stuck on stars.
#define CURSOR_SNAP_AREA (RES_CASE(0,3,6))

static void
UpdateCursorInfo (UNICODE *prevbuf)
{	
	// JMS: Display hint for the player to use the star search facility
	UNICODE buf[CURSOR_INFO_BUFSIZE];
	POINT pt;
	STAR_DESC *SDPtr;
	STAR_DESC *BestSDPtr;

	// "(Star search: default F6)"
	utf8StringCopy (buf, sizeof (buf), GAME_STRING (FEEDBACK_STRING_BASE + 2));
        
	pt.x = UNIVERSE_TO_DISPX (cursorLoc.x);
	pt.y = UNIVERSE_TO_DISPY (cursorLoc.y);

	SDPtr = BestSDPtr = 0;
	
	while ((SDPtr = FindStar (SDPtr, &cursorLoc, 75, 75)))
	{
		if ((UNIVERSE_TO_DISPX (SDPtr->star_pt.x) >= pt.x - CURSOR_SNAP_AREA && UNIVERSE_TO_DISPX (SDPtr->star_pt.x) <= pt.x + CURSOR_SNAP_AREA)
				&& (UNIVERSE_TO_DISPY (SDPtr->star_pt.y) >= pt.y - CURSOR_SNAP_AREA && UNIVERSE_TO_DISPY (SDPtr->star_pt.y) <= pt.y + CURSOR_SNAP_AREA)
				&& (BestSDPtr == 0 || STAR_TYPE (SDPtr->Type) >= STAR_TYPE (BestSDPtr->Type)))
			BestSDPtr = SDPtr;
	}

	if (BestSDPtr)
	{	// A star is near the cursor:
		// Snap cursor onto star only in 1x res. In hi-res modes,  
		// snapping is done when the star is selected as auto-pilot target.
		if (RESOLUTION_FACTOR == 0)
			cursorLoc = BestSDPtr->star_pt;
		
		GetClusterName (BestSDPtr, buf);
	}
	else
	{	// No star found. Reset the coordinates to the cursor's location
		cursorLoc.x = DISP_TO_UNIVERSEX (pt.x);
		if (cursorLoc.x < 0)
			cursorLoc.x = 0;
		else if (cursorLoc.x > MAX_X_UNIVERSE)
			cursorLoc.x = MAX_X_UNIVERSE;
		cursorLoc.y = DISP_TO_UNIVERSEY (pt.y);
		if (cursorLoc.y < 0)
			cursorLoc.y = 0;
		else if (cursorLoc.y > MAX_Y_UNIVERSE)
			cursorLoc.y = MAX_Y_UNIVERSE;
	}

	if (GET_GAME_STATE (ARILOU_SPACE))
	{
		POINT ari_pt;

		if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
		{
			ari_pt.x = ARILOU_SPACE_X;
			ari_pt.y = ARILOU_SPACE_Y;
		}
		else
		{
			ari_pt.x = QUASI_SPACE_X;
			ari_pt.y = QUASI_SPACE_Y;
		}

		if (UNIVERSE_TO_DISPX (ari_pt.x) == pt.x
				&& UNIVERSE_TO_DISPY (ari_pt.y) == pt.y)
		{
			cursorLoc = ari_pt;
			utf8StringCopy (buf, sizeof (buf),
					GAME_STRING (STAR_STRING_BASE + 132));
		}
	}

	LockMutex (GraphicsLock);
	DrawHyperCoords (cursorLoc);
	if (strcmp (buf, prevbuf) != 0)
	{
		strcpy (prevbuf, buf);
		DrawSISMessage (buf);
	}
	UnlockMutex (GraphicsLock);
}

static void
UpdateFuelRequirement (void)
{
	UNICODE buf[80];
	COUNT fuel_required;
	DWORD f;
	POINT pt;

	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
		pt = CurStarDescPtr->star_pt;
	else
	{
		pt.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
		pt.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
	}
	pt.x -= cursorLoc.x;
	pt.y -= cursorLoc.y;

	f = (DWORD)((long)pt.x * pt.x + (long)pt.y * pt.y);
	if (f == 0 || GET_GAME_STATE (ARILOU_SPACE_SIDE) > 1 || GET_GAME_STATE (ORZ_SPACE_SIDE) > 1) // JMS: ORZ space check
		fuel_required = 0;
	else
		fuel_required = square_root (f) + (FUEL_TANK_SCALE / 20);

	sprintf (buf, "%s %u.%u",
			GAME_STRING (NAVIGATION_STRING_BASE + 4),
			fuel_required / FUEL_TANK_SCALE,
			(fuel_required % FUEL_TANK_SCALE) / 10);

	LockMutex (GraphicsLock);
	DrawStatusMessage (buf);
	UnlockMutex (GraphicsLock);
}

#define STAR_SEARCH_BUFSIZE 256

typedef struct starsearch_state
{
	// TODO: pMS field is probably not needed anymore
	MENU_STATE *pMS;
	UNICODE Text[STAR_SEARCH_BUFSIZE];
	UNICODE LastText[STAR_SEARCH_BUFSIZE];
	DWORD LastChangeTime;
	int FirstIndex;
	int CurIndex;
	int LastIndex;
	BOOLEAN SingleClust;
	BOOLEAN SingleMatch;
	UNICODE Buffer[STAR_SEARCH_BUFSIZE];
	const UNICODE *Prefix;
	const UNICODE *Cluster;
	int PrefixLen;
	int ClusterLen;
	int ClusterPos;
	int SortedStars[NUM_SOLAR_SYSTEMS];
} STAR_SEARCH_STATE;

static int
compStarName (const void *ptr1, const void *ptr2)
{
	int index1;
	int index2;

	index1 = *(const int *) ptr1;
	index2 = *(const int *) ptr2;
	if (star_array[index1].Postfix != star_array[index2].Postfix)
	{
		return utf8StringCompare (GAME_STRING (star_array[index1].Postfix),
				GAME_STRING (star_array[index2].Postfix));
	}

	if (star_array[index1].Prefix < star_array[index2].Prefix)
		return -1;
	
	if (star_array[index1].Prefix > star_array[index2].Prefix)
		return 1;

	return 0;
}

static void
SortStarsOnName (STAR_SEARCH_STATE *pSS)
{
	int i;
	int *sorted = pSS->SortedStars;

	for (i = 0; i < NUM_SOLAR_SYSTEMS; i++)
		sorted[i] = i;

	qsort (sorted, NUM_SOLAR_SYSTEMS, sizeof (int), compStarName);
}

static void
SplitStarName (STAR_SEARCH_STATE *pSS)
{
	UNICODE *buf = pSS->Buffer;
	UNICODE *next;
	UNICODE *sep = NULL;

	pSS->Prefix = 0;
	pSS->PrefixLen = 0;
	pSS->Cluster = 0;
	pSS->ClusterLen = 0;
	pSS->ClusterPos = 0;

	// skip leading space
	for (next = buf; *next != '\0' &&
			getCharFromString ((const UNICODE **)&next) == ' ';
			buf = next)
		;
	if (*buf == '\0')
	{	// no text
		return;
	}

	pSS->Prefix = buf;

	// See if player gave a prefix
	for (buf = next; *next != '\0' &&
			getCharFromString ((const UNICODE **)&next) != ' ';
			buf = next)
		;
	if (*buf != '\0')
	{	// found possibly separating ' '
		sep = buf;
		// skip separating space
		for (buf = next; *next != '\0' &&
				getCharFromString ((const UNICODE **)&next) == ' ';
				buf = next)
			;
	}

	if (*buf == '\0')
	{	// reached the end -- cluster only
		pSS->Cluster = pSS->Prefix;
		pSS->ClusterLen = utf8StringCount (pSS->Cluster);
		pSS->ClusterPos = utf8StringCountN (pSS->Buffer, pSS->Cluster);
		pSS->Prefix = 0;
		return;
	}

	// consider the rest cluster name (whatever there is)
	pSS->Cluster = buf;
	pSS->ClusterLen = utf8StringCount (pSS->Cluster);
	pSS->ClusterPos = utf8StringCountN (pSS->Buffer, pSS->Cluster);
	*sep = '\0'; // split
	pSS->PrefixLen = utf8StringCount (pSS->Prefix);
}

static inline int
SkipStarCluster (int *sortedStars, int istar)
{
	int Postfix = star_array[sortedStars[istar]].Postfix;

	for (++istar; istar < NUM_SOLAR_SYSTEMS &&
			star_array[sortedStars[istar]].Postfix == Postfix;
			++istar)
		;
	return istar;
}

static int
FindNextStarIndex (STAR_SEARCH_STATE *pSS, int from, BOOLEAN WithinClust)
{
	int i;

	if (!pSS->Cluster)
		return -1; // nothing to search for

	for (i = from; i < NUM_SOLAR_SYSTEMS; ++i)
	{
		STAR_DESC *SDPtr = &star_array[pSS->SortedStars[i]];
		UNICODE FullName[STAR_SEARCH_BUFSIZE];
		UNICODE *ClusterName = GAME_STRING (SDPtr->Postfix);
		const UNICODE *sptr;
		const UNICODE *dptr;
		int dlen;
		int c;
		
		dlen = utf8StringCount (ClusterName);
		if (pSS->ClusterLen > dlen)
		{	// no match, skip the rest of cluster
			i = SkipStarCluster (pSS->SortedStars, i) - 1;
			continue;
		}

		for (c = 0, sptr = pSS->Cluster, dptr = ClusterName;
				c < pSS->ClusterLen; ++c)
		{
			UniChar sc = getCharFromString (&sptr);
			UniChar dc = getCharFromString (&dptr);

			if (UniChar_toUpper (sc) != UniChar_toUpper (dc))
				break;
		}

		if (c < pSS->ClusterLen)
		{	// no match here, skip the rest of cluster
			i = SkipStarCluster (pSS->SortedStars, i) - 1;
			continue;
		}

		if (pSS->Prefix && !SDPtr->Prefix)
			// we were given a prefix but found a singular star;
			// that is a no match
			continue;

		if (WithinClust)
			// searching within clusters; any prefix is a match
			break;

		if (!pSS->Prefix)
		{	// searching for cluster name only
			// return only the first stars in a cluster
			if (i == 0 || SDPtr->Postfix !=
					star_array[pSS->SortedStars[i - 1]].Postfix)
			{	// found one
				break;
			}
			else
			{	// another star in the same cluster, skip cluster
				i = SkipStarCluster (pSS->SortedStars, i) - 1;
				continue;
			}
		}

		// check prefix
		GetClusterName (SDPtr, FullName);
		dlen = utf8StringCount (FullName);
		if (pSS->PrefixLen > dlen)
			continue;

		for (c = 0, sptr = pSS->Prefix, dptr = FullName;
				c < pSS->PrefixLen; ++c)
		{
			UniChar sc = getCharFromString (&sptr);
			UniChar dc = getCharFromString (&dptr);

			if (UniChar_toUpper (sc) != UniChar_toUpper (dc))
				break;
		}

		if (c >= pSS->PrefixLen)
			break; // found one
	}

	return (i < NUM_SOLAR_SYSTEMS) ? i : -1;
}

static void
DrawMatchedStarName (TEXTENTRY_STATE *pTES)
{
	STAR_SEARCH_STATE *pSS = (STAR_SEARCH_STATE *) pTES->CbParam;
	UNICODE buf[STAR_SEARCH_BUFSIZE] = "";
	SIZE ExPos = 0;
	SIZE CurPos = -1;
	STAR_DESC *SDPtr = &star_array[pSS->SortedStars[pSS->CurIndex]];
	COUNT flags;

	if (pSS->SingleClust || pSS->SingleMatch)
	{	// draw full star name
		GetClusterName (SDPtr, buf);
		ExPos = -1;
		flags = DSME_SETFR;
	}
	else
	{	// draw substring match
		UNICODE *pstr = buf;

		strcpy (pstr, pSS->Text);
		ExPos = pSS->ClusterPos;
		pstr = skipUTF8Chars (pstr, pSS->ClusterPos);

		strcpy (pstr, GAME_STRING (SDPtr->Postfix));
		ExPos += pSS->ClusterLen;
		CurPos = pTES->CursorPos;

		flags = DSME_CLEARFR;
		if (pTES->JoystickMode)
			flags |= DSME_BLOCKCUR;
	}
	
	LockMutex (GraphicsLock);
	DrawSISMessageEx (buf, CurPos, ExPos, flags);
	DrawHyperCoords (cursorLoc);
	UnlockMutex (GraphicsLock);
}

static void
MatchNextStar (STAR_SEARCH_STATE *pSS, BOOLEAN Reset)
{
	if (Reset)
		pSS->FirstIndex = -1; // reset cache
	
	if (pSS->FirstIndex < 0)
	{	// first time after changes
		pSS->CurIndex = -1;
		pSS->LastIndex = -1;
		pSS->SingleClust = FALSE;
		pSS->SingleMatch = FALSE;
		strcpy (pSS->Buffer, pSS->Text);
		SplitStarName (pSS);
	}

	pSS->CurIndex = FindNextStarIndex (pSS, pSS->CurIndex + 1,
			pSS->SingleClust);
	if (pSS->FirstIndex < 0) // first search
		pSS->FirstIndex = pSS->CurIndex;
	
	if (pSS->CurIndex >= 0)
	{	// remember as last (searching forward-only)
		pSS->LastIndex = pSS->CurIndex;
	}
	else
	{	// wrap around
		pSS->CurIndex = pSS->FirstIndex;

		if (pSS->FirstIndex == pSS->LastIndex && pSS->FirstIndex != -1)
		{
			if (!pSS->Prefix)
			{	// only one cluster matching
				pSS->SingleClust = TRUE;
			}
			else
			{	// exact match
				pSS->SingleMatch = TRUE;
			}
		}
	}
}

static BOOLEAN
OnStarNameChange (TEXTENTRY_STATE *pTES)
{
	STAR_SEARCH_STATE *pSS = (STAR_SEARCH_STATE *) pTES->CbParam;
	COUNT flags;
	BOOLEAN ret = TRUE;

	if (strcmp (pSS->Text, pSS->LastText) != 0)
	{	// string changed
		pSS->LastChangeTime = GetTimeCounter ();
		strcpy (pSS->LastText, pSS->Text);
		
		// reset the search
		MatchNextStar (pSS, TRUE);
	}

	if (pSS->CurIndex < 0)
	{	// nothing found
		if (pSS->Text[0] == '\0')
			flags = DSME_SETFR;
		else
			flags = DSME_CLEARFR;
		if (pTES->JoystickMode)
			flags |= DSME_BLOCKCUR;

		LockMutex (GraphicsLock);
		ret = DrawSISMessageEx (pSS->Text, pTES->CursorPos, -1, flags);
		UnlockMutex (GraphicsLock);
	}
	else
	{
		STAR_DESC *SDPtr;

		// move the cursor to the found star
		SDPtr = &star_array[pSS->SortedStars[pSS->CurIndex]];
		UpdateCursorLocation (0, 0, &SDPtr->star_pt);

		DrawMatchedStarName (pTES);
		UpdateFuelRequirement ();
	}

	return ret;
}

static BOOLEAN
OnStarNameFrame (TEXTENTRY_STATE *pTES)
{
	STAR_SEARCH_STATE *pSS = (STAR_SEARCH_STATE *) pTES->CbParam;

	if (PulsedInputState.menu[KEY_MENU_NEXT])
	{	// search for next match
		STAR_DESC *SDPtr;

		MatchNextStar (pSS, FALSE);

		if (pSS->CurIndex < 0)
		{	// nothing found
			if (PulsedInputState.menu[KEY_MENU_NEXT])
				PlayMenuSound (MENU_SOUND_FAILURE);
			return TRUE;
		}

		// move the cursor to the found star
		SDPtr = &star_array[pSS->SortedStars[pSS->CurIndex]];
		UpdateCursorLocation (0, 0, &SDPtr->star_pt);

		DrawMatchedStarName (pTES);
		UpdateFuelRequirement ();
	}

	flashCurrentLocation (NULL);

	SleepThread (ONE_SECOND / 30);
	
	return TRUE;
}

static BOOLEAN
DoStarSearch (MENU_STATE *pMS)
{
	TEXTENTRY_STATE tes;
	STAR_SEARCH_STATE *pss;
	BOOLEAN success;

	pss = HMalloc (sizeof (*pss));
	if (!pss)
		return FALSE;

	LockMutex (GraphicsLock);
	DrawSISMessageEx ("", 0, 0, DSME_SETFR);
	UnlockMutex (GraphicsLock);

	pss->pMS = pMS;
	pss->LastChangeTime = 0;
	pss->Text[0] = '\0';
	pss->LastText[0] = '\0';
	pss->FirstIndex = -1;
	SortStarsOnName (pss);

	// text entry setup
	tes.Initialized = FALSE;
	tes.BaseStr = pss->Text;
	tes.MaxSize = sizeof (pss->Text);
	tes.CursorPos = 0;
	tes.CbParam = pss;
	tes.ChangeCallback = OnStarNameChange;
	tes.FrameCallback = OnStarNameFrame;

	SetMenuSounds (MENU_SOUND_ARROWS, MENU_SOUND_SELECT);
	SetDefaultMenuRepeatDelay ();
	success = DoTextEntry (&tes);

	LockMutex (GraphicsLock);
	DrawSISMessageEx (pss->Text, -1, -1, DSME_CLEARFR);
	UnlockMutex (GraphicsLock);

	HFree (pss);

	return success;
} 

static BOOLEAN
DoMoveCursor (MENU_STATE *pMS)
{
#define MIN_ACCEL_DELAY_1X (ONE_SECOND / 60)
#define MAX_ACCEL_DELAY_1X (ONE_SECOND / 8)
#define STEP_ACCEL_DELAY_1X (ONE_SECOND / 120)
#define MIN_ACCEL_DELAY_2X (ONE_SECOND / 60)
#define MAX_ACCEL_DELAY_2X (ONE_SECOND / 10)
#define STEP_ACCEL_DELAY_2X (ONE_SECOND / 120)
#define MIN_ACCEL_DELAY_4X (ONE_SECOND / 60)
#define MAX_ACCEL_DELAY_4X (ONE_SECOND / 10)
#define STEP_ACCEL_DELAY_4X (ONE_SECOND / 120)

	static UNICODE last_buf[CURSOR_INFO_BUFSIZE];
	DWORD TimeIn = GetTimeCounter ();
	static COUNT moveRepeats;
	BOOLEAN isMove = FALSE;
	
	COUNT MIN_ACCEL_DELAY, MAX_ACCEL_DELAY, STEP_ACCEL_DELAY;
	
	if (RESOLUTION_FACTOR == 0)
	{
		MIN_ACCEL_DELAY = MIN_ACCEL_DELAY_1X;
		MAX_ACCEL_DELAY = MAX_ACCEL_DELAY_1X;
		STEP_ACCEL_DELAY = STEP_ACCEL_DELAY_1X;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		MIN_ACCEL_DELAY = MIN_ACCEL_DELAY_2X;
		MAX_ACCEL_DELAY = MAX_ACCEL_DELAY_2X;
		STEP_ACCEL_DELAY = STEP_ACCEL_DELAY_2X;
	}
	else
	{
		MIN_ACCEL_DELAY = MIN_ACCEL_DELAY_4X;
		MAX_ACCEL_DELAY = MAX_ACCEL_DELAY_4X;
		STEP_ACCEL_DELAY = STEP_ACCEL_DELAY_4X;
	}
	

	if (!pMS->Initialized)
	{
		POINT universe;

		pMS->Initialized = TRUE;
		pMS->InputFunc = DoMoveCursor;

		if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
			universe = CurStarDescPtr->star_pt;
		else
		{
			universe.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
			universe.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
		}
		flashCurrentLocation (&universe);

		last_buf[0] = '\0';
		UpdateCursorInfo (last_buf);
		UpdateFuelRequirement ();

		return TRUE;
	}
	else if (PulsedInputState.menu[KEY_MENU_CANCEL])
	{
		return FALSE;
	}
	else if (PulsedInputState.menu[KEY_MENU_SELECT])
	{
		// JMS: The hi-res modes now have a user-friendly starmap cursor.
		// The cursor finds a star even if the cursor is several pixels away from it (CURSOR_SNAP_AREA)
		// The cursor centers on the star only when selected as an auto-pilot target.
		if (RESOLUTION_FACTOR > 0)
		{
			STAR_DESC *SDPtr;
			STAR_DESC *BestSDPtr;
			POINT pt;
			
			pt.x = UNIVERSE_TO_DISPX (cursorLoc.x);
			pt.y = UNIVERSE_TO_DISPY (cursorLoc.y);
			SDPtr = BestSDPtr = 0;
			
			while ((SDPtr = FindStar (SDPtr, &cursorLoc, 75, 75)))
			{
				if ((UNIVERSE_TO_DISPX (SDPtr->star_pt.x) >= pt.x - CURSOR_SNAP_AREA && UNIVERSE_TO_DISPX (SDPtr->star_pt.x) <= pt.x + CURSOR_SNAP_AREA)
					&& (UNIVERSE_TO_DISPY (SDPtr->star_pt.y) >= pt.y -CURSOR_SNAP_AREA && UNIVERSE_TO_DISPY (SDPtr->star_pt.y) <= pt.y + CURSOR_SNAP_AREA)
					&& (BestSDPtr == 0 || STAR_TYPE (SDPtr->Type) >= STAR_TYPE (BestSDPtr->Type)))
					BestSDPtr = SDPtr;
			}
			
			if (BestSDPtr)
			{
				cursorLoc = BestSDPtr->star_pt;
				UpdateCursorLocation (0, 0, &BestSDPtr->star_pt);
			}
		}
		
		GLOBAL (autopilot) = cursorLoc;
        if (GET_GAME_STATE (ORZ_SPACE_SIDE) > 1)
        {
            SetOrzSpaceAutoPilot();
        }

#ifdef DEBUG
		if (instantMove)
		{
			PlayMenuSound (MENU_SOUND_INVOKED);

			if (LOBYTE (GLOBAL (CurrentActivity)) == IN_HYPERSPACE)
			{
				// Move to the new location immediately.
				doInstantMove ();
			}
			else if (LOBYTE (GLOBAL (CurrentActivity)) == IN_INTERPLANETARY)
			{
				// We're in a solar system; exit it.
				GLOBAL (CurrentActivity) |= END_INTERPLANETARY;
			
				// Set a hook to move to the new location:
				debugHook = doInstantMove;
			}

			return FALSE;
		}
#endif
		DrawStarMap (0, NULL);
	}
	else if (PulsedInputState.menu[KEY_MENU_SEARCH])
	{
		if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
		{	// HyperSpace search
			POINT oldpt = cursorLoc;

			if (!DoStarSearch (pMS))
			{	// search failed or canceled - return cursor
				UpdateCursorLocation (0, 0, &oldpt);
			}
			// make sure cmp fails
			strcpy (last_buf, "  <random garbage>  ");
			UpdateCursorInfo (last_buf);
			UpdateFuelRequirement ();

			SetMenuRepeatDelay (MIN_ACCEL_DELAY, MAX_ACCEL_DELAY, STEP_ACCEL_DELAY, TRUE);
			SetMenuSounds (MENU_SOUND_NONE, MENU_SOUND_NONE);
		}
		else
		{	// no search in QuasiSpace
			PlayMenuSound (MENU_SOUND_FAILURE);
		}
	}
	else
	{
		SBYTE sx, sy;
		SIZE ZoomIn, ZoomOut;

		ZoomIn = ZoomOut = 0;
		if (PulsedInputState.menu[KEY_MENU_ZOOM_IN])
			ZoomIn = 1;
		else if (PulsedInputState.menu[KEY_MENU_ZOOM_OUT])
			ZoomOut = 1;

		ZoomStarMap (ZoomIn - ZoomOut);

		sx = sy = 0;
		if (PulsedInputState.menu[KEY_MENU_LEFT])    sx =   -1;
		if (PulsedInputState.menu[KEY_MENU_RIGHT])   sx =    1;
		if (PulsedInputState.menu[KEY_MENU_UP])      sy =   -1;
		if (PulsedInputState.menu[KEY_MENU_DOWN])    sy =    1;

		if (moveRepeats > 25)
		{
			sx *= RES_CASE(1,2,2);
			sy *= RES_CASE(1,2,2);
		}
		if (moveRepeats > 50)
		{
			sx *= RES_CASE(1,1,2);
			sy *= RES_CASE(1,1,2);
		}
		
		// BW: we need to go through this because 4x only checks for
		// input every ONE_SECOND/40 or so, thus reducing
		// MIN_ACCEL_DELAY is of no use. In practice it's similar.
		
		if (sx != 0 || sy != 0)
		{
			UpdateCursorLocation (sx, sy, NULL);
			UpdateCursorInfo (last_buf);
			UpdateFuelRequirement ();
			isMove = TRUE;
		}
	
		SleepThreadUntil (TimeIn + MIN_ACCEL_DELAY);
	}
	
	if (isMove)
		++moveRepeats;
	else
		moveRepeats = 0;

	flashCurrentLocation (NULL);

	return !(GLOBAL (CurrentActivity) & CHECK_ABORT);
}

static void
RepairMap (COUNT update_race, RECT *pLastRect, RECT *pNextRect)
{
	RECT r;

	/* make a rect big enough for text */
	r.extent.width = 50;
	r.corner.x = (pNextRect->corner.x + (pNextRect->extent.width >> 1))
			- (r.extent.width >> 1);
	if (r.corner.x < 0)
		r.corner.x = 0;
	else if (r.corner.x + r.extent.width >= SIS_SCREEN_WIDTH)
		r.corner.x = SIS_SCREEN_WIDTH - r.extent.width;
	r.extent.height = 9;
	r.corner.y = (pNextRect->corner.y + (pNextRect->extent.height >> 1))
			- r.extent.height;
	if (r.corner.y < 0)
		r.corner.y = 0;
	else if (r.corner.y + r.extent.height >= SIS_SCREEN_HEIGHT)
		r.corner.y = SIS_SCREEN_HEIGHT - r.extent.height;
	BoxUnion (pLastRect, &r, &r);
	BoxUnion (pNextRect, &r, &r);
	*pLastRect = *pNextRect;

	if (r.corner.x < 0)
	{
		r.extent.width += r.corner.x;
		r.corner.x = 0;
	}
	if (r.corner.x + r.extent.width > SIS_SCREEN_WIDTH)
		r.extent.width = SIS_SCREEN_WIDTH - r.corner.x;
	if (r.corner.y < 0)
	{
		r.extent.height += r.corner.y;
		r.corner.y = 0;
	}
	if (r.corner.y + r.extent.height > SIS_SCREEN_HEIGHT)
		r.extent.height = SIS_SCREEN_HEIGHT - r.corner.y;

	r.extent.height += r.corner.y & 1;
	r.corner.y &= ~1;
	
	DrawStarMap (update_race, &r);
}

static void
UpdateMap (void)
{
	BYTE ButtonState, VisibleChange;
	BOOLEAN MapDrawn, Interrupted;
	COUNT index;
	HFLEETINFO hStarShip, hNextShip;

	FlushInput ();
	ButtonState = 1; /* assume a button down */

	MapDrawn = Interrupted = FALSE;
	for (index = 1,
			hStarShip = GetHeadLink (&GLOBAL (avail_race_q));
			hStarShip; ++index, hStarShip = hNextShip)
	{
		FLEET_INFO *FleetPtr;

		FleetPtr = LockFleetInfo (&GLOBAL (avail_race_q), hStarShip);
		hNextShip = _GetSuccLink (FleetPtr);

		if (ButtonState)
		{
			if (!AnyButtonPress (TRUE))
				ButtonState = 0;
		}
		else if ((Interrupted = (BOOLEAN)(Interrupted || AnyButtonPress (TRUE))))
			MapDrawn = TRUE;

		if (FleetPtr->known_strength)
		{
			SIZE dx, dy, delta;
			RECT r, last_r, temp_r0, temp_r1;

			dx = FleetPtr->loc.x - FleetPtr->known_loc.x;
			dy = FleetPtr->loc.y - FleetPtr->known_loc.y;
			if (dx || dy)
			{
				SIZE xincr, yincr,
						xerror, yerror,
						cycle;

				if (dx >= 0)
					xincr = 1;
				else
				{
					xincr = -1;
					dx = -dx;
				}
				dx <<= 1;

				if (dy >= 0)
					yincr = 1;
				else
				{
					yincr = -1;
					dy = -dy;
				}
				dy <<= 1;

				if (dx >= dy)
					cycle = dx;
				else
					cycle = dy;
				delta = xerror = yerror = cycle >> 1;

				if (!MapDrawn)
				{
					DrawStarMap ((COUNT)~0, NULL);
					MapDrawn = TRUE;
				}

				GetSphereRect (FleetPtr, &temp_r0, &last_r);
				++last_r.extent.width;
				++last_r.extent.height;
				VisibleChange = FALSE;
				do
				{
					do
					{
						if ((xerror -= dx) <= 0)
						{
							FleetPtr->known_loc.x += xincr;
							xerror += cycle;
						}
						if ((yerror -= dy) <= 0)
						{
							FleetPtr->known_loc.y += yincr;
							yerror += cycle;
						}
						GetSphereRect (FleetPtr, &temp_r1, &r);
					} while (delta--
							&& ((delta & 0x1F)
							|| (temp_r0.corner.x == temp_r1.corner.x
							&& temp_r0.corner.y == temp_r1.corner.y)));

					if (ButtonState)
					{
						if (!AnyButtonPress (TRUE))
							ButtonState = 0;
					}
					else if ((Interrupted = (BOOLEAN)(
								Interrupted || AnyButtonPress (TRUE)
								)))
					{
						MapDrawn = TRUE;
						goto DoneSphereMove;
					}

					++r.extent.width;
					++r.extent.height;
					if (temp_r0.corner.x != temp_r1.corner.x
							|| temp_r0.corner.y != temp_r1.corner.y)
					{
						VisibleChange = TRUE;
						RepairMap (index, &last_r, &r);
					}
				} while (delta >= 0);
				if (VisibleChange)
					RepairMap ((COUNT)~0, &last_r, &r);

DoneSphereMove:
				FleetPtr->known_loc = FleetPtr->loc;
			}

			// JMS: Negative known strength marks initial state of the SoI's: 
			// Only the home planet of certain races is known -> show small sphere.
			if (FleetPtr->known_strength > 0)
				delta = FleetPtr->actual_strength - FleetPtr->known_strength;
			else if (FleetPtr->known_strength < 0)
				delta = 10;
			
			if (delta)
			{
				if (!MapDrawn)
				{
					DrawStarMap ((COUNT)~0, NULL);
					MapDrawn = TRUE;
				}

				if (delta > 0)
					dx = 1;
				else
				{
					delta = -delta;
					dx = -1;
				}
				--delta;

				GetSphereRect (FleetPtr, &temp_r0, &last_r);
				++last_r.extent.width;
				++last_r.extent.height;
				VisibleChange = FALSE;
				do
				{
					do
					{
						// JMS: Negative known strength marks initial state of the SoI's: 
						// Only the home planet of certain races is known -> show small sphere.
						if (FleetPtr->known_strength > 0)
							FleetPtr->known_strength += dx;
						
						GetSphereRect (FleetPtr, &temp_r1, &r);
					} while (delta--
							&& ((delta & 0xF)
							|| temp_r0.extent.height == temp_r1.extent.height));

					if (ButtonState)
					{
						if (!AnyButtonPress (TRUE))
							ButtonState = 0;
					}
					else if ((Interrupted = (BOOLEAN)(Interrupted || AnyButtonPress (TRUE))))
					{
						MapDrawn = TRUE;
						goto DoneSphereGrowth;
					}
					++r.extent.width;
					++r.extent.height;
					if (temp_r0.extent.height != temp_r1.extent.height)
					{
						VisibleChange = TRUE;
						RepairMap (index, &last_r, &r);
					}
				} while (delta >= 0);
				if (VisibleChange || temp_r0.extent.width != temp_r1.extent.width)
					RepairMap ((COUNT)~0, &last_r, &r);

DoneSphereGrowth:
				// JMS: Negative known strength marks initial state of the SoI's: 
				// Only the home planet of certain races is known -> show small sphere.
				if (FleetPtr->known_strength > 0)
					FleetPtr->known_strength = FleetPtr->actual_strength;
			}
		}

		UnlockFleetInfo (&GLOBAL (avail_race_q), hStarShip);
	}
}

BOOLEAN
StarMap (void)
{
	MENU_STATE MenuState;
	POINT universe;
	//FRAME OldFrame;
	RECT clip_r;
	CONTEXT OldContext;

	COUNT MIN_ACCEL_DELAY, MAX_ACCEL_DELAY, STEP_ACCEL_DELAY;
	
	if (RESOLUTION_FACTOR == 0)
	{
		MIN_ACCEL_DELAY = MIN_ACCEL_DELAY_1X;
		MAX_ACCEL_DELAY = MAX_ACCEL_DELAY_1X;
		STEP_ACCEL_DELAY = STEP_ACCEL_DELAY_1X;
	}
	else if (RESOLUTION_FACTOR == 1)
	{
		MIN_ACCEL_DELAY = MIN_ACCEL_DELAY_2X;
		MAX_ACCEL_DELAY = MAX_ACCEL_DELAY_2X;
		STEP_ACCEL_DELAY = STEP_ACCEL_DELAY_2X;
	}
	else
	{
		MIN_ACCEL_DELAY = MIN_ACCEL_DELAY_4X;
		MAX_ACCEL_DELAY = MAX_ACCEL_DELAY_4X;
		STEP_ACCEL_DELAY = STEP_ACCEL_DELAY_4X;
	}
	
	memset (&MenuState, 0, sizeof (MenuState));

	zoomLevel = 0;
	mapOrigin.x = MAX_X_UNIVERSE >> 1;
	mapOrigin.y = MAX_Y_UNIVERSE >> 1;
	StarMapFrame = SetAbsFrameIndex (MiscDataFrame, 48);

	if (LOBYTE (GLOBAL (CurrentActivity)) != IN_HYPERSPACE)
		universe = CurStarDescPtr->star_pt;
	else
	{
		universe.x = LOGX_TO_UNIVERSE (GLOBAL_SIS (log_x));
		universe.y = LOGY_TO_UNIVERSE (GLOBAL_SIS (log_y));
	}

	cursorLoc = GLOBAL (autopilot);
	if (cursorLoc.x == ~0 && cursorLoc.y == ~0)
		cursorLoc = universe;

	MenuState.InputFunc = DoMoveCursor;
	MenuState.Initialized = FALSE;

	transition_pending = TRUE;
	if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
		UpdateMap ();

	LockMutex (GraphicsLock);
	
	DrawStarMap (0, (RECT*)-1);
	transition_pending = FALSE;
	
	BatchGraphics ();
	OldContext = SetContext (SpaceContext);
	GetContextClipRect (&clip_r);
	SetContext (OldContext);
	LoadIntoExtraScreen (&clip_r);
	DrawCursor (UNIVERSE_TO_DISPX (cursorLoc.x),
			UNIVERSE_TO_DISPY (cursorLoc.y));
	UnbatchGraphics ();
	UnlockMutex (GraphicsLock);

	SetMenuSounds (MENU_SOUND_NONE, MENU_SOUND_NONE);
	SetMenuRepeatDelay (MIN_ACCEL_DELAY, MAX_ACCEL_DELAY, STEP_ACCEL_DELAY,
			TRUE);
	DoInput (&MenuState, FALSE);
	SetMenuSounds (MENU_SOUND_ARROWS, MENU_SOUND_SELECT);
	SetDefaultMenuRepeatDelay ();

	LockMutex (GraphicsLock);
	DrawHyperCoords (universe);
	DrawSISMessage (NULL);
	DrawStatusMessage (NULL);
	UnlockMutex (GraphicsLock);

	if (GLOBAL (autopilot.x) == universe.x
			&& GLOBAL (autopilot.y) == universe.y)
		GLOBAL (autopilot.x) = GLOBAL (autopilot.y) = ~0;

	return (GLOBAL (autopilot.x) != ~0
			&& GLOBAL (autopilot.y) != ~0);
}

