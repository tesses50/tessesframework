/*
    TessesFramework a library to make C++ easier for me, used in CrossLang:
   https://git.tesses.org/tesses50/crosslang Copyright (C) 2026 Mike Nolan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "Args.hpp"
#include "BitTorrent/TorrentFile.hpp"
#include "Crypto/ClientTLSStream.hpp"
#include "Crypto/Crypto.hpp"
#include "Date/Date.hpp"
#include "Filesystem/FSHelpers.hpp"
#include "Filesystem/LocalFS.hpp"
#include "Filesystem/MountableFilesystem.hpp"
#include "Filesystem/NullFilesystem.hpp"
#include "Filesystem/RelativeFilesystem.hpp"
#include "Filesystem/SubdirFilesystem.hpp"
#include "HiddenField.hpp"
#include "Http/BasicAuthServer.hpp"
#include "Http/CGIServer.hpp"
#include "Http/CallbackServer.hpp"
#include "Http/ChangeableServer.hpp"
#include "Http/ContentDisposition.hpp"
#include "Http/FileServer.hpp"
#include "Http/HttpClient.hpp"
#include "Http/HttpServer.hpp"
#include "Http/MountableServer.hpp"
#include "Http/RouteServer.hpp"
#include "Lazy.hpp"
#include "Mail/Smtp.hpp"
#include "Platform/Environment.hpp"
#include "Platform/Process.hpp"
#include "Random.hpp"
#include "Serialization/Bencode.hpp"
#include "Serialization/BitConverter.hpp"
#include "Serialization/Json.hpp"
#include "Serialization/SQLite.hpp"
#include "Streams/BufferedStream.hpp"
#include "Streams/ByteReader.hpp"
#include "Streams/ByteWriter.hpp"
#include "Streams/FileStream.hpp"
#include "Streams/MemoryStream.hpp"
#include "Streams/NetworkStream.hpp"
#include "Text/HeaderGenerator.hpp"
#include "Text/StringConverter.hpp"
#include "TextStreams/StdIOReader.hpp"
#include "TextStreams/StdIOWriter.hpp"
#include "TextStreams/StreamReader.hpp"
#include "TextStreams/StreamWriter.hpp"
#include "TextStreams/StringReader.hpp"
#include "TextStreams/StringWriter.hpp"
#include "Threading/Mutex.hpp"
#include "Threading/Thread.hpp"
#include "Threading/ThreadPool.hpp"
#include "Console.hpp"