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
#ifndef N_SCRIPT_PARSER_H
#define N_SCRIPT_PARSER_H

#include "ASTNode.h"
#include "Tokenizer.h"
#include <n/core/Array.h>

namespace n {
namespace script {

class SynthaxErrorException : public std::exception
{
	public:
		SynthaxErrorException(const core::Array<Token::Type> &e, const Token &t) : expected(e), token(t) {
		}

		virtual const char *what() const noexcept override;
		virtual const char *what(const core::String &code) const noexcept;

		const Token &getToken() const;
		const core::Array<Token::Type> &getExpected() const;

	private:
		core::Array<Token::Type> expected;
		Token token;

		mutable core::String buffer;
};

class Parser : NonCopyable
{
	public:
		Parser();


		ASTStatement *parse(const core::Array<Token> &tokens) const {
			return parse(tokens.begin(), tokens.end());
		}

		ASTStatement *parse(core::Array<Token>::const_iterator begin, core::Array<Token>::const_iterator end) const;

};

}
}

#endif // N_SCRIPT_PARSER_H
