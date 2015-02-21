/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>
Copyright (C) 2015 nerzhul, Loic Blot <loic.blot@unix-experience.fr>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "serveropcodes.h"

const static ToServerCommandHandler null_command_handler = { "TOSERVER_NULL", TOSERVER_STATE_ALL, &Server::handleCommand_Null };

const ToServerCommandHandler toServerCommandTable[TOSERVER_NUM_MSG_TYPES] =
{
#if TODO
	null_command_handler, // 0x00
	null_command_handler, // 0x01
	null_command_handler, // 0x02
	null_command_handler, // 0x03
	null_command_handler, // 0x04
	null_command_handler, // 0x05
	null_command_handler, // 0x06
	null_command_handler, // 0x07
	null_command_handler, // 0x08
	null_command_handler, // 0x09
	null_command_handler, // 0x0a
	null_command_handler, // 0x0b
	null_command_handler, // 0x0c
	null_command_handler, // 0x0d
	null_command_handler, // 0x0e
	null_command_handler, // 0x0f
	{ "TOSERVER_INIT",                     TOSERVER_STATE_NOT_CONNECTED, &Server::handleCommand_Init }, // 0x10
	{ "TOSERVER_INIT2",                    TOSERVER_STATE_NOT_CONNECTED, &Server::handleCommand_Init2 }, // 0x11
	null_command_handler, // 0x12
	null_command_handler, // 0x13
	null_command_handler, // 0x14
	null_command_handler, // 0x15
	null_command_handler, // 0x16
	null_command_handler, // 0x17
	null_command_handler, // 0x18
	null_command_handler, // 0x19
	null_command_handler, // 0x1a
	null_command_handler, // 0x1b
	null_command_handler, // 0x1c
	null_command_handler, // 0x1d
	null_command_handler, // 0x1e
	null_command_handler, // 0x1f
	null_command_handler, // 0x20
	null_command_handler, // 0x21
	null_command_handler, // 0x22
	{ "TOSERVER_PLAYERPOS",                TOSERVER_STATE_INGAME, &Server::handleCommand_PlayerPos }, // 0x23
	{ "TOSERVER_GOTBLOCKS",                TOSERVER_STATE_STARTUP, &Server::handleCommand_GotBlocks }, // 0x24
	{ "TOSERVER_DELETEDBLOCKS",            TOSERVER_STATE_INGAME, &Server::handleCommand_DeletedBlocks }, // 0x25
	null_command_handler, // 0x26
	{ "TOSERVER_CLICK_OBJECT",             TOSERVER_STATE_INGAME, &Server::handleCommand_Deprecated }, // 0x27
	{ "TOSERVER_GROUND_ACTION",            TOSERVER_STATE_INGAME, &Server::handleCommand_Deprecated }, // 0x28
	{ "TOSERVER_RELEASE",                  TOSERVER_STATE_INGAME, &Server::handleCommand_Deprecated }, // 0x29
	null_command_handler, // 0x2a
	null_command_handler, // 0x2b
	null_command_handler, // 0x2c
	null_command_handler, // 0x2d
	null_command_handler, // 0x2e
	null_command_handler, // 0x2f
	{ "TOSERVER_SIGNTEXT",                 TOSERVER_STATE_INGAME, &Server::handleCommand_Deprecated }, // 0x30
	{ "TOSERVER_INVENTORY_ACTION",         TOSERVER_STATE_INGAME, &Server::handleCommand_InventoryAction }, // 0x31
	{ "TOSERVER_CHAT_MESSAGE",             TOSERVER_STATE_INGAME, &Server::handleCommand_ChatMessage }, // 0x32
	{ "TOSERVER_SIGNNODETEXT",             TOSERVER_STATE_INGAME, &Server::handleCommand_Deprecated }, // 0x33
	{ "TOSERVER_CLICK_ACTIVEOBJECT",       TOSERVER_STATE_INGAME, &Server::handleCommand_Deprecated }, // 0x34
	{ "TOSERVER_DAMAGE",                   TOSERVER_STATE_INGAME, &Server::handleCommand_Damage }, // 0x35
	{ "TOSERVER_PASSWORD",                 TOSERVER_STATE_INGAME, &Server::handleCommand_Password }, // 0x36
	{ "TOSERVER_PLAYERITEM",               TOSERVER_STATE_INGAME, &Server::handleCommand_PlayerItem }, // 0x37
	{ "TOSERVER_RESPAWN",                  TOSERVER_STATE_INGAME, &Server::handleCommand_Respawn }, // 0x38
	{ "TOSERVER_INTERACT",                 TOSERVER_STATE_INGAME, &Server::handleCommand_Interact }, // 0x39
	{ "TOSERVER_REMOVED_SOUNDS",           TOSERVER_STATE_INGAME, &Server::handleCommand_RemovedSounds }, // 0x3a
	{ "TOSERVER_NODEMETA_FIELDS",          TOSERVER_STATE_INGAME, &Server::handleCommand_NodeMetaFields }, // 0x3b
	{ "TOSERVER_INVENTORY_FIELDS",         TOSERVER_STATE_INGAME, &Server::handleCommand_InventoryFields }, // 0x3c
	null_command_handler, // 0x3d
	null_command_handler, // 0x3e
	null_command_handler, // 0x3f
	{ "TOSERVER_REQUEST_MEDIA",            TOSERVER_STATE_STARTUP, &Server::handleCommand_RequestMedia }, // 0x40
	{ "TOSERVER_RECEIVED_MEDIA",           TOSERVER_STATE_STARTUP, &Server::handleCommand_ReceivedMedia }, // 0x41
	{ "TOSERVER_BREATH",                   TOSERVER_STATE_INGAME, &Server::handleCommand_Breath }, // 0x42
	{ "TOSERVER_CLIENT_READY",             TOSERVER_STATE_STARTUP, &Server::handleCommand_ClientReady }, // 0x43
#endif
};
