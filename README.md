# DungeonViewerAC
A Dungeon Viewer for Asheron's Call

# Summary
This is a mostly complete dungeon viewer that can work with Asheron's Call versions both old and new. It can render dungeons, their objects, particle effects/scripts, and utilize the BSP tree for efficiency.

# How to use
There are two prebuilt binaries in the Build folder for convenience sake.

1. Copy your Asheron's Call data files to the Dungeon Viewer folder.
2. Run either: 
 * DungeonViewer_DM.exe for the Dark Majesty version. This uses portal.dat and cell.dat files.
 * DungeonViewer_ToD.exe for the Throne of Destiny. This uses client_portal.dat and client_cell_1.dat files.

# Known issues
The code was originally written prior to 2004 and has been partially updated to Throne of Destiny, with certain texture code not updated (if textures are missing, that portion of the code needs updating.)

# For developers
The PRE_TOD define located in StdAfx.h determines whether the build is for old (not Throne of Destiny) or new (Throne of Destiny.) The define is used throughout the application in various spots where the data formats may have changed.

