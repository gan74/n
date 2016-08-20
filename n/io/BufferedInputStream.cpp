/*******************************
Copyright (C) 2013-2016 gregoire ANGERAND

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
**********************************/

#include "BufferedInputStream.h"
#include <n/defines.h>

namespace n {
namespace io {

BufferedInputStream::BufferedInputStream(InputStream *st, uint si) : stream(st), bufferSize(si), bufferUsed(0), buffer(new char[bufferSize]) {
}

BufferedInputStream::~BufferedInputStream() {
	delete[] buffer;
}

bool BufferedInputStream::atEnd() const {
	return stream->atEnd() && !bufferUsed;
}

bool BufferedInputStream::canRead() const {
	return (stream->canRead() || bufferUsed) && !atEnd();
}

uint BufferedInputStream::getBufferUsedSize() const {
	return bufferUsed;
}

uint BufferedInputStream::readBytes(void *b, uint l) {
	uint offset = bufferSize - bufferUsed;
	if(l > bufferUsed || l == uint(-1)) {
		memcpy(b, buffer + offset, bufferUsed);
		uint read = bufferUsed;
		bufferUsed = 0;
		if(l != uint(-1) && l < bufferSize + read) {
			if(!fillBuffer()) {
				return read;
			}
			return read + readBytes(reinterpret_cast<byte *>(b) + read, l - read);
		} else {
			return stream->readBytes(reinterpret_cast<byte *>(b) + read, -1) + read;
		}
	} else {
		memcpy(b, buffer + offset, l);
		bufferUsed -= l;
		return l;
	}
}

uint BufferedInputStream::fillBuffer() {
	bufferUsed = stream->readBytes(buffer, bufferSize);
	uint offset = bufferSize - bufferUsed;
	if(offset) {
		memmove(buffer + offset, buffer, bufferUsed);
	}
	return bufferUsed;
}

}
}

