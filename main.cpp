#include <n/core/String.h>
#include <n/script/Parser.h>
#include <n/core/Map.h>
#include <n/core/Timer.h>
#include <iostream>
#include <n/script/WTNode.h>
#include <n/script/WTBuilder.h>
#include <n/script/BytecodeCompiler.h>
#include <n/script/Machine.h>

using namespace n;
using namespace n::core;
using namespace n::script;

static_assert(isLittleEndian(), "not lilendian");


void print(uint index, BytecodeInstruction i);

int fib(volatile int a) {
	if(a < 1) return 1;
	return fib(a - 1) + fib(a - 2);
}


int main(int, char **) {
	core::String code = "def fib(var a:Int) = {"
						"if(a < 1) return 1;"
						"return fib(a - 1) + fib(a - 2);"
						"}"
						"fib(32);"
						//"var f:Float;"
						//"fib(f);"
						;


	Tokenizer tokenizer;
	auto tks = tokenizer.tokenize(code);

	Parser parser;
	WTBuilder builder;

	try {
		ASTInstruction *node = parser.parse(tks);
		std::cout << node->toString() << std::endl << std::endl << std::endl << std::endl;

		node->lookupFunctions(builder);
		WTInstruction *wt = node->toWorkTree(builder);

		BytecodeCompiler compiler;
		BytecodeAssembler ass = compiler.compile(wt, builder.getTypeSystem());


		uint index = 0;
		for(BytecodeInstruction i : ass.getInstructions()) {
			if(i.op == Bytecode::FuncHead1) {
				index = 0;
			}
			print(index++, i);
		}

		Timer timer;

		Machine machine;
		machine.load(ass.getInstructions().begin(), ass.getInstructions().end());
		Primitive ret = machine.run(ass.getInstructions().begin());

		double time = timer.reset();
		int cret = fib(32);
		double ctime = timer.reset();
		std::cout << std::endl << "return " << ret.integer << " expected " << cret << std::endl << "eval = " << time * 1000 << "ms (vs " << ctime * 1000 << "ms)" << std::endl << std::endl;
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
	names[Bytecode::LessI] = "lessi";
	names[Bytecode::GreaterI] = "gri";
	names[Bytecode::Equals] = "eq";
	names[Bytecode::NotEq] = "neq";
	switch(i.op) {
		case Bytecode::SetI:
			std::cout << "seti $" << i.dst << " " << i.data;
		break;

		case Bytecode::SetF:
			std::cout << "setf $" << i.dst << " " << i.data;
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

		case Bytecode::Call:
			std::cout << "call $" << i.dst << " " << i.udata + 1;
		break;

		case Bytecode::PushArg:
			std::cout << "push $" << i.dst;
		break;

		case Bytecode::FuncHead1:
			std::cout << "function1";
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
