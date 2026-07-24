/**************************************************************************/
/*  FBXUtil.h                                                             */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GNUCHAN ENGINE                             */
/*                    https://github.com/gnuchanos/GnuChan_Engine         */
/**************************************************************************/
/* Copyright (C) 2025 GnuChan Engine contributors (see AUTHORS.md).      */
/*                                                                        */
/* This program is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or      */
/* (at your option) any later version.                                    */
/*                                                                        */
/* This program is distributed in the hope that it will be useful,        */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of         */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the           */
/* GNU General Public License for more details.                           */
/*                                                                        */
/* You should have received a copy of the GNU General Public License      */
/* along with this program. If not, see <https://www.gnu.org/licenses/>.  */
/**************************************************************************/

/*
Open Asset Import Library (assimp)
----------------------------------------------------------------------

Copyright (c) 2006-2019, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

/** @file  FBXUtil.h
 *  @brief FBX utility functions for internal use
 */

#ifndef FBXUTIL_H
#define FBXUTIL_H

#include "FBXTokenizer.h"
#include <stdint.h>

namespace FBXDocParser {

namespace Util {

/** Get a string representation for a #TokenType. */
const char *TokenTypeString(TokenType t);

/** Decode a single Base64-encoded character.
 *
 *  @param ch Character to decode (from base64 to binary).
 *  @return decoded byte value*/
uint8_t DecodeBase64(char ch);

/** Compute decoded size of a Base64-encoded string
 *
 *  @param in Characters to decode.
 *  @param inLength Number of characters to decode.
 *  @return size of the decoded data (number of bytes)*/
size_t ComputeDecodedSizeBase64(const char *in, size_t inLength);

/** Decode a Base64-encoded string
 *
 *  @param in Characters to decode.
 *  @param inLength Number of characters to decode.
 *  @param out Pointer where we will store the decoded data.
 *  @param maxOutLength Size of output buffer.
 *  @return size of the decoded data (number of bytes)*/
size_t DecodeBase64(const char *in, size_t inLength, uint8_t *out, size_t maxOutLength);

char EncodeBase64(char byte);

/** Encode bytes in base64-encoding
 *
 *  @param data Binary data to encode.
 *  @param inLength Number of bytes to encode.
 *  @return base64-encoded string*/
std::string EncodeBase64(const char *data, size_t length);

} // namespace Util
} // namespace FBXDocParser

#endif // FBXUTIL_H
