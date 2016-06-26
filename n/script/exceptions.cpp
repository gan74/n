/*******************************
Copyright (C) 2013-2015 gregoire ANGERAND

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
#include "exceptions.h"

namespace n {
namespace script {

const char *SynthaxErrorException::what() const noexcept {
	buffer = "{ ";
	for(Token::Type t : expected) {
		buffer << Token::getName(t) << " ";
	}
	buffer = "Expected " + buffer + "} got " + Token::getName(token.type) + " (\"" + token.string + "\")";
	return buffer.data();
}

const char *SynthaxErrorException::what(const core::String &code) const noexcept {
	buffer = "{ ";
	for(Token::Type t : expected) {
		buffer << Token::getName(t) << " ";
	}
	buffer = "Expected " + buffer + "} got " + Token::getName(token.type) + ":";
	if(token.type == Token::Identifier) {
		buffer += " \"" + token.string + "\"";
	}
	buffer += "\n" + token.position.toString(code);

	return buffer.data();
}

const Token &SynthaxErrorException::getToken() const {
	return token;
}

const core::Array<Token::Type> &SynthaxErrorException::getExpected() const {
	return expected;
}

const char *ValidationErrorException::what() const noexcept {
	return msg.data();
}

const char *ValidationErrorException::what(const core::String &code) const noexcept {
	buffer = msg + "\n" + position.toString(code);
	return buffer.data();
}


}
}
