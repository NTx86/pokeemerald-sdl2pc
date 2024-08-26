#include "global.h"
#include "berry.h"
#include "bike.h"
#include "field_camera.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "event_object_movement.h"
#include "gpu_regs.h"
#include "menu.h"
#include "overworld.h"
#include "rotating_gate.h"
#include "sprite.h"
#include "text.h"

EWRAM_DATA bool8 gUnusedBikeCameraAheadPanback = FALSE;

struct FieldCameraOffset
{
    u32 xPixelOffset;
    u32 yPixelOffset;
};

static void DrawWholeMapViewInternal(const struct MapLayout *);
static void DrawMetatileAt(const struct MapLayout *, int, int);
static void DrawMetatile(s32, const u16 *, size_t, size_t, u32);
static void CameraPanningCB_PanAhead(void);

static struct FieldCameraOffset sFieldCameraOffset;
static s16 sHorizontalCameraPan;
static s16 sVerticalCameraPan;
static bool8 sBikeCameraPanFlag;
static void (*sFieldCameraPanningCallback)(void);

struct CameraObject gFieldCamera;
u16 gTotalCameraPixelOffsetY;
u16 gTotalCameraPixelOffsetX;

static void ResetCameraOffset(struct FieldCameraOffset *cameraOffset)
{
    cameraOffset->xPixelOffset = gSaveBlock1Ptr->pos.x * 16;
    cameraOffset->yPixelOffset = gSaveBlock1Ptr->pos.y * 16;
}

static void AddCameraPixelOffset(struct FieldCameraOffset *cameraOffset, u32 xOffset, u32 yOffset)
{
    cameraOffset->xPixelOffset += xOffset;
    cameraOffset->yPixelOffset += yOffset;
}

void ResetFieldCamera(void)
{
    ResetCameraOffset(&sFieldCameraOffset);
}

static bool8 UseFullScreen(void)
{
    if (GetGpuState(GPU_STATE_DISPCNT) & DISPCNT_GBA_MODE)
        return FALSE;

    return TRUE;
}

void FieldUpdateBgTilemapScroll(void)
{
    u32 r4, r5;
    r5 = sFieldCameraOffset.xPixelOffset + sHorizontalCameraPan + 128;
    r4 = sVerticalCameraPan + sFieldCameraOffset.yPixelOffset + 8;

    if (UseFullScreen())
    {
        r5 -= (DisplayWidth() - BASE_DISPLAY_WIDTH) / 2;
        r4 -= (DisplayHeight() - BASE_DISPLAY_HEIGHT) / 2;
    }

    SetGpuBackgroundX(1, r5);
    SetGpuBackgroundY(1, r4);
    SetGpuBackgroundX(2, r5);
    SetGpuBackgroundY(2, r4);
    SetGpuBackgroundX(3, r5);
    SetGpuBackgroundY(3, r4);
}

void GetCameraOffsetWithPan(s16 *x, s16 *y)
{
    *x = sFieldCameraOffset.xPixelOffset + sHorizontalCameraPan;
    *y = sFieldCameraOffset.yPixelOffset + sVerticalCameraPan + 8;
}

void DrawWholeMapView(void)
{
    DrawWholeMapViewInternal(gMapHeader.mapLayout);
}

static void DrawWholeMapViewInternal(const struct MapLayout *mapLayout)
{
    u32 x, y;
    
    ReInitOverworldBgs();

    for (y = 0; y < mapLayout->height + MAP_OFFSET_H; y++)
    {
        for (x = 0; x < mapLayout->width + MAP_OFFSET_W; x++)
        {
            DrawMetatileAt(mapLayout, x, y);
        }
    }
}

void CurrentMapDrawMetatileAt(int x, int y)
{
    DrawMetatileAt(gMapHeader.mapLayout, x, y);
}

void DrawDoorMetatileAt(int x, int y, u16 *tiles)
{
    DrawMetatile(METATILE_LAYER_TYPE_COVERED, tiles, x, y, 0);
}

static void DrawMetatileAt(const struct MapLayout *mapLayout, int x, int y)
{
    u32 tileSetOffset = 0;
    u16 metatileId;
    const u16 *metatiles;

    if (x < 0 || x >= mapLayout->width + MAP_OFFSET_W)
        return;
    if (y < 0 || y >= mapLayout->height + MAP_OFFSET_H)
        return;

    metatileId = MapGridGetMetatileIdAt(x, y);

    if (metatileId > NUM_METATILES_TOTAL)
        metatileId = 0;
    if (metatileId < NUM_METATILES_IN_PRIMARY)
        metatiles = mapLayout->primaryTileset->metatiles;
    else
    {
        u8 connId = MapGridGetConnectionId(x, y);
        
        if (connId == 0 || (connId-1) > gMapHeader.connections->count)
        {
            metatiles = mapLayout->secondaryTileset->metatiles;
        }
        else
        {
            const struct MapConnection *connection = &gMapHeader.connections->connections[connId-1];
            struct MapHeader const *cMap = GetMapHeaderFromConnection(connection);
            
            metatiles = cMap->mapLayout->secondaryTileset->metatiles;
            tileSetOffset = NUM_METATILES_TOTAL + NUM_METATILES_TOTAL * (connId-1);
        }
        metatileId -= NUM_METATILES_IN_PRIMARY;
    }

    DrawMetatile(MapGridGetMetatileLayerTypeAt(x, y), metatiles + (metatileId * NUM_TILES_PER_METATILE), (size_t)x, (size_t)y, tileSetOffset);
}

#define Convert16TileTo32Tile(tile, offset) ( ((tile & 0x3FF) + offset) | ((tile << 6) & 0x30000) | ((tile << 6) & 0x3C0000 ) )

static void DrawMetatile(s32 metatileLayerType, const u16 *tiles, size_t x, size_t y, u32 tileSetOffset)
{
    size_t a, b, c, d;
    size_t stride = gOverworldTilemapWidth;

    x *= 2;
    y *= 2;

    a = (y * stride) + x;
    b = a + 1;
    c = a + stride;
    d = c + 1;

    switch (metatileLayerType)
    {
    case METATILE_LAYER_TYPE_SPLIT:
        // Draw metatile's bottom layer to the bottom background layer.
        gOverworldTilemapBuffer_Bg3[a] = Convert16TileTo32Tile(tiles[0], tileSetOffset);
        gOverworldTilemapBuffer_Bg3[b] = Convert16TileTo32Tile(tiles[1], tileSetOffset);
        gOverworldTilemapBuffer_Bg3[c] = Convert16TileTo32Tile(tiles[2], tileSetOffset);
        gOverworldTilemapBuffer_Bg3[d] = tiles[3];

        // Draw transparent tiles to the middle background layer.
        gOverworldTilemapBuffer_Bg2[a] = 0;
        gOverworldTilemapBuffer_Bg2[b] = 0;
        gOverworldTilemapBuffer_Bg2[c] = 0;
        gOverworldTilemapBuffer_Bg2[d] = 0;

        // Draw metatile's top layer to the top background layer.
        gOverworldTilemapBuffer_Bg1[a] = Convert16TileTo32Tile(tiles[4], tileSetOffset);
        gOverworldTilemapBuffer_Bg1[b] = Convert16TileTo32Tile(tiles[5], tileSetOffset);
        gOverworldTilemapBuffer_Bg1[c] = Convert16TileTo32Tile(tiles[6], tileSetOffset);
        gOverworldTilemapBuffer_Bg1[d] = Convert16TileTo32Tile(tiles[7], tileSetOffset);
        break;
    case METATILE_LAYER_TYPE_COVERED:
        // Draw metatile's bottom layer to the bottom background layer.
        gOverworldTilemapBuffer_Bg3[a] = Convert16TileTo32Tile(tiles[0], tileSetOffset);
        gOverworldTilemapBuffer_Bg3[b] = Convert16TileTo32Tile(tiles[1], tileSetOffset);
        gOverworldTilemapBuffer_Bg3[c] = Convert16TileTo32Tile(tiles[2], tileSetOffset);
        gOverworldTilemapBuffer_Bg3[d] = Convert16TileTo32Tile(tiles[3], tileSetOffset);

        // Draw metatile's top layer to the middle background layer.
        gOverworldTilemapBuffer_Bg2[a] = Convert16TileTo32Tile(tiles[4], tileSetOffset);
        gOverworldTilemapBuffer_Bg2[b] = Convert16TileTo32Tile(tiles[5], tileSetOffset);
        gOverworldTilemapBuffer_Bg2[c] = Convert16TileTo32Tile(tiles[6], tileSetOffset);
        gOverworldTilemapBuffer_Bg2[d] = Convert16TileTo32Tile(tiles[7], tileSetOffset);

        // Draw transparent tiles to the top background layer.
        gOverworldTilemapBuffer_Bg1[a] = 0;
        gOverworldTilemapBuffer_Bg1[b] = 0;
        gOverworldTilemapBuffer_Bg1[c] = 0;
        gOverworldTilemapBuffer_Bg1[d] = 0;
        break;
    case METATILE_LAYER_TYPE_NORMAL:
        // Draw garbage to the bottom background layer.
        gOverworldTilemapBuffer_Bg3[a] = 0x3014;
        gOverworldTilemapBuffer_Bg3[b] = 0x3014;
        gOverworldTilemapBuffer_Bg3[c] = 0x3014;
        gOverworldTilemapBuffer_Bg3[d] = 0x3014;

        // Draw metatile's bottom layer to the middle background layer.
        gOverworldTilemapBuffer_Bg2[a] = Convert16TileTo32Tile(tiles[0], tileSetOffset);
        gOverworldTilemapBuffer_Bg2[b] = Convert16TileTo32Tile(tiles[1], tileSetOffset);
        gOverworldTilemapBuffer_Bg2[c] = Convert16TileTo32Tile(tiles[2], tileSetOffset);
        gOverworldTilemapBuffer_Bg2[d] = Convert16TileTo32Tile(tiles[3], tileSetOffset);

        // Draw metatile's top layer to the top background layer, which covers object event sprites.
        gOverworldTilemapBuffer_Bg1[a] = Convert16TileTo32Tile(tiles[4], tileSetOffset);
        gOverworldTilemapBuffer_Bg1[b] = Convert16TileTo32Tile(tiles[5], tileSetOffset);
        gOverworldTilemapBuffer_Bg1[c] = Convert16TileTo32Tile(tiles[6], tileSetOffset);
        gOverworldTilemapBuffer_Bg1[d] = Convert16TileTo32Tile(tiles[7], tileSetOffset);
        break;
    }
    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);
    ScheduleBgCopyTilemapToVram(3);
}

static void CameraUpdateCallback(struct CameraObject *fieldCamera)
{
    if (fieldCamera->spriteId != 0)
    {
        fieldCamera->movementSpeedX = gSprites[fieldCamera->spriteId].data[2];
        fieldCamera->movementSpeedY = gSprites[fieldCamera->spriteId].data[3];
    }
}

void ResetCameraUpdateInfo(void)
{
    gFieldCamera.movementSpeedX = 0;
    gFieldCamera.movementSpeedY = 0;
    gFieldCamera.x = 0;
    gFieldCamera.y = 0;
    gFieldCamera.spriteId = 0;
    gFieldCamera.callback = NULL;
}

u32 InitCameraUpdateCallback(u8 trackedSpriteId)
{
    if (gFieldCamera.spriteId != 0)
        DestroySprite(&gSprites[gFieldCamera.spriteId]);
    gFieldCamera.spriteId = AddCameraObject(trackedSpriteId);
    gFieldCamera.callback = CameraUpdateCallback;
    return 0;
}

void CameraUpdate(void)
{
    int deltaX;
    int deltaY;
    int curMovementOffsetY;
    int curMovementOffsetX;
    int movementSpeedX;
    int movementSpeedY;

    if (gFieldCamera.callback != NULL)
        gFieldCamera.callback(&gFieldCamera);
    movementSpeedX = gFieldCamera.movementSpeedX;
    movementSpeedY = gFieldCamera.movementSpeedY;
    deltaX = 0;
    deltaY = 0;
    curMovementOffsetX = gFieldCamera.x;
    curMovementOffsetY = gFieldCamera.y;


    if (curMovementOffsetX == 0 && movementSpeedX != 0)
    {
        if (movementSpeedX > 0)
            deltaX = 1;
        else
            deltaX = -1;
    }
    if (curMovementOffsetY == 0 && movementSpeedY != 0)
    {
        if (movementSpeedY > 0)
            deltaY = 1;
        else
            deltaY = -1;
    }
    if (curMovementOffsetX != 0 && curMovementOffsetX == -movementSpeedX)
    {
        if (movementSpeedX > 0)
            deltaX = 1;
        else
            deltaX = -1;
    }
    if (curMovementOffsetY != 0 && curMovementOffsetY == -movementSpeedY)
    {
        if (movementSpeedY > 0)
            deltaX = 1;
        else
            deltaX = -1;
    }

    gFieldCamera.x += movementSpeedX;
    gFieldCamera.x %= 16;
    gFieldCamera.y += movementSpeedY;
    gFieldCamera.y %= 16;

    if (deltaX != 0 || deltaY != 0)
    {
        CameraMove(deltaX, deltaY);
        UpdateObjectEventsForCameraUpdate(deltaX, deltaY);
        RotatingGatePuzzleCameraUpdate(deltaX, deltaY);
        SetBerryTreesSeen();
    }

    AddCameraPixelOffset(&sFieldCameraOffset, movementSpeedX, movementSpeedY);
    gTotalCameraPixelOffsetX -= movementSpeedX;
    gTotalCameraPixelOffsetY -= movementSpeedY;
}

void MoveCameraAndRedrawMap(int deltaX, int deltaY) //unused
{
    CameraMove(deltaX, deltaY);
    UpdateObjectEventsForCameraUpdate(deltaX, deltaY);
    DrawWholeMapView();
    gTotalCameraPixelOffsetX -= deltaX * 16;
    gTotalCameraPixelOffsetY -= deltaY * 16;
}

void SetCameraPanningCallback(void (*callback)(void))
{
    sFieldCameraPanningCallback = callback;
}

void SetCameraPanning(s16 horizontal, s16 vertical)
{
    sHorizontalCameraPan = horizontal;
    sVerticalCameraPan = vertical + 32;
}

void InstallCameraPanAheadCallback(void)
{
    sFieldCameraPanningCallback = CameraPanningCB_PanAhead;
    sBikeCameraPanFlag = FALSE;
    sHorizontalCameraPan = 0;
    sVerticalCameraPan = 32;
}

void UpdateCameraPanning(void)
{
    if (sFieldCameraPanningCallback != NULL)
        sFieldCameraPanningCallback();
    //Update sprite offset of overworld objects
    gSpriteCoordOffsetX = gTotalCameraPixelOffsetX - sHorizontalCameraPan - 128;
    gSpriteCoordOffsetY = gTotalCameraPixelOffsetY - sVerticalCameraPan - 8;

    if (UseFullScreen())
    {
        gSpriteCoordOffsetX += (DisplayWidth() - BASE_DISPLAY_WIDTH) / 2;
        gSpriteCoordOffsetY += (DisplayHeight() - BASE_DISPLAY_HEIGHT) / 2;
    }
}

static void CameraPanningCB_PanAhead(void)
{
    u8 var;

    if (gUnusedBikeCameraAheadPanback == FALSE)
    {
        InstallCameraPanAheadCallback();
    }
    else
    {
        // this code is never reached
        if (gPlayerAvatar.tileTransitionState == T_TILE_TRANSITION)
        {
            sBikeCameraPanFlag ^= 1;
            if (sBikeCameraPanFlag == FALSE)
                return;
        }
        else
        {
            sBikeCameraPanFlag = FALSE;
        }

        var = GetPlayerMovementDirection();
        if (var == 2)
        {
            if (sVerticalCameraPan > -8)
                sVerticalCameraPan -= 2;
        }
        else if (var == 1)
        {
            if (sVerticalCameraPan < 72)
                sVerticalCameraPan += 2;
        }
        else if (sVerticalCameraPan < 32)
        {
            sVerticalCameraPan += 2;
        }
        else if (sVerticalCameraPan > 32)
        {
            sVerticalCameraPan -= 2;
        }
    }
}
