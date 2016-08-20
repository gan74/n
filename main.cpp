#include <n/core/String.h>
#include <n/script/Parser.h>
#include <n/core/Map.h>
#include <n/core/Timer.h>
#include <n/io/File.h>
#include <n/io/TextInputStream.h>
#include <n/io/Dir.h>
#include <n/script/WTNode.h>
#include <n/script/ClassBuilder.h>
#include <n/script/exceptions.h>
#include <n/script/BytecodeCompiler.h>
#include <n/script/Machine.h>

#include <cstring>
#include <iostream>

using namespace n;
using namespace n::core;
using namespace n::script;
using namespace n::io;

static_assert(isLittleEndian(), "not lilendian");


void print(uint index, BytecodeInstruction i);
void test();
void interpret();

int64 fib(volatile int64 a) {
	if(a < 1) return 1;
	return fib(a - 1) + fib(a - 2);
}

/*void run(ASTStatement *node, ClassBuilder &builder) {
	node->lookupFunctions(builder);
	WTStatement *wt = node->toWorkTree(builder);

	BytecodeCompiler compiler;
	auto p = compiler.compile(wt, &builder.getTypeSystem()).getInstructions();

	Machine m;
	m.load(p.begin(), p.end());
	m.run(p.begin());
}*/



int main(int, char **) {
	core::String code = "class App = "
						"{"
							"def fib(a:Int):Int = {"
								"if(a < 1) return 1;"
								"return fib(a - 1) + fib(a - 2);"
							"}"
							"def fib2():Int = {"
								"return fib(32);"
							"}"
						"}"
						"class main = {"
							"def main():Int = {"
								"var x:Int = 32;"
								"var app:App;"
								"x = app.fib2();"
								"return x;"
							"}"
						"}"
						;


	Tokenizer tokenizer;
	auto tks = tokenizer.tokenize(code);

	Parser parser;

	TypeSystem types;
	ObjectDataType global("the global scope");

	try {
		ASTStatement *node = parser.parse(tks);
		std::cout << node->toString() << std::endl << std::endl << std::endl << std::endl;

		node->lookupTypes(types);

		ClassBuilder builder(&types, &global);
		node->lookupFunctions(builder);
		WTStatement *wt = node->toWorkTree(builder);

		BytecodeCompiler compiler;
		BytecodeAssembler ass = compiler.compile(wt, &types);


		uint index = 0;
		for(uint j = 0; j != ass.getInstructions().size(); j++) {
			BytecodeInstruction i = ass.getInstructions()[j];
			if(i.op == Bytecode::Constants) {
				std::cout << "\tconstants " << i.dst << std::endl;
				const BytecodeInstruction *data = &ass.getInstructions()[j + 1];
				for(uint k = 0; k != i.dst; k++) {
					core::String string(reinterpret_cast<const char *>(data));
					std::cout << "\t  \"" << string << "\"" << std::endl;
					uint si = string.size() + 1;
					data += si / sizeof(BytecodeInstruction);
					data += !!(si % sizeof(BytecodeInstruction));
				}
				j += i.udata;
				continue;
			}
			if(i.op == Bytecode::FuncHead1) {
				index = 0;
			}
			print(index++, i);
		}

		Timer timer;

		Machine machine;
		machine.load(ass.getInstructions().begin(), ass.getInstructions().end());
		Primitive ret = machine.run();

		double time = timer.reset();
		int64 cret = fib(32);
		double ctime = timer.reset();
		std::cout << std::endl << "return " << int64(ret.integer) << " expected " << cret << std::endl << "eval = " << time * 1000 << "ms (vs " << ctime * 1000 << "ms)" << std::endl << std::endl;
	} catch(SynthaxErrorException &e) {
		std::cerr << e.what(code) << std::endl;
	} catch(ValidationErrorException &e) {
		std::cerr << e.what(code) << std::endl;
	}


	return 0;
}






void print(uint index, BytecodeInstruction i) {
	std::cout << index<< "\t";
	Map<Bytecode, core::String> names;
	names[Bytecode::AddI] = "addi";
	names[Bytecode::SubI] = "subi";
	names[Bytecode::MulI] = "muli";
	names[Bytecode::DivI] = "divi";

	names[Bytecode::AddF] = "addf";
	names[Bytecode::SubF] = "subf";
	names[Bytecode::MulF] = "mulf";
	names[Bytecode::DivF] = "divf";

	names[Bytecode::LessI] = "lessi";
	names[Bytecode::GreaterI] = "gri";

	names[Bytecode::LessF] = "lessf";
	names[Bytecode::GreaterF] = "grf";

	names[Bytecode::Equals] = "eq";
	names[Bytecode::NotEq] = "neq";
	switch(i.op) {
		case Bytecode::Constants:
		break;

		case Bytecode::ClassHead:
			std::cout << "class " << i.dst;
		break;

		case Bytecode::SetI:
			std::cout << "seti $" << i.dst << " " << i.data;
		break;

		case Bytecode::SetF:
			std::cout << "setf $" << i.dst << " " << i.data;
		break;

		case Bytecode::New:
			std::cout << "new $" << i.dst;
		break;

		case Bytecode::ToFloat:
			std::cout << "float $" << i.dst << " $" << i.src[0];
		break;

		case Bytecode::Copy:
			std::cout << "cpy $" << i.dst << " $" << i.src[0];
		break;

		case Bytecode::Not:
			std::cout << "not $" << i.dst << " $" << i.src[0];
		break;

		case Bytecode::Jump:
			std::cout << "jmp " << i.udata + 1;
		break;

		case Bytecode::JumpZ:
			std::cout << "jmpz $" << i.dst << " " << i.udata + 1;
		break;

		case Bytecode::JumpNZ:
			std::cout << "jmpnz $" << i.dst << " " << i.udata + 1;
		break;

		case Bytecode::CallVirtual:
			std::cout << "callvirt $" << i.dst << " $" << i.src[0] << " " << i.src[1] + 1;
		break;

		/*case Bytecode::InvokeStatic:
			std::cout << "ivkstatic $" << i.dst << " " << i.src[0] << " " << i.src[1] + 1;
		break;*/

		case Bytecode::PushArg:
			std::cout << "push $" << i.dst;
		break;

		case Bytecode::FuncHead1:
			std::cout << "function1 " << i.dst << " " << i.src[0];
		break;

		case Bytecode::FuncHead2:
			std::cout << "function2 " << i.dst << " " << i.src[0];
		break;

		case Bytecode::Ret:
			std::cout << "ret $" << i.dst;
		break;

		case Bytecode::RetI:
			std::cout << "retIm " << i.data;
		break;

		case Bytecode::Exit:
			std::cout << "exit";
		break;

		default:
			std::cout << names[i.op] << " $" << i.dst << " $" << i.src[0] << " $" << i.src[1];


	}
	std::cout << std::endl;
}
