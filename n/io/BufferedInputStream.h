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

#ifndef N_IO_BUFFEREDINPUTSTREAM_H
#define N_IO_BUFFEREDINPUTSTREAM_H

#include "InputStream.h"
#include <n/utils.h>

namespace n {
namespace io {

class BufferedInputStream : public InputStream
{
	public:
		BufferedInputStream(InputStream *st, uint si = 64 * 1024);
		~BufferedInputStream();

		bool atEnd() const override;
		bool canRead() const override;
		uint readBytes(void *b, uint l = -1) override;

		uint getBufferUsedSize() const;

	private:
		uint fillBuffer();

		InputStream *stream;
		uint bufferSize;
		uint bufferUsed;
		char *buffer;
};

}
}

#endif // N_IO_BUFFEREDINPUTSTREAM_H
