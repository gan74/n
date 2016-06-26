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
#include "BytecodeCompiler.h"
#include "exceptions.h"

namespace n {
namespace script {

template<typename T, typename U>
T *as(U *n) {
	return static_cast<T *>(n);
}


BytecodeCompiler::BytecodeCompiler() {
}

BytecodeAssembler BytecodeCompiler::compile(WTStatement *node, TypeSystem *ts) {
	BytecodeAssembler assembler;
	Context context{
		core::Map<WTFunction *, BytecodeAssembler>(),
		&assembler,
		ts,
		false};

	compile(context, node);

	assembler.ret(0);
	assembler.exit();

	for(const core::Pair<WTFunction * const, BytecodeAssembler> &p : context.externalAssemblers) {
		assembler << p._2;
	}

	return assembler;
}

void BytecodeCompiler::compile(Context &context, WTStatement *node) {
	switch(node->type) {

		case WTNode::Block:
			for(WTStatement *i : as<wt::Block>(node)->instructions) {
				compile(context, i);
			}
		return;


		case WTNode::Expression:
			compile(context, as<wt::ExprInstr>(node)->expression);
		return;


		case WTNode::Loop: {
			BytecodeAssembler::Label loop = context.assembler->createLabel();
			compile(context, as<wt::Loop>(node)->condition);

			BytecodeAssembler::Label condJmp = context.assembler->createLabel();
			context.assembler->nope();

			compile(context, as<wt::Loop>(node)->body);

			if(context.useIfDoWhile) {
				compile(context, as<wt::Loop>(node)->condition);
				context.assembler->jumpNZ(as<wt::Loop>(node)->condition->registerIndex, loop);
			} else {
				context.assembler->jump(loop);
			}

			context.assembler->seek(condJmp);
			context.assembler->jumpZ(as<wt::Loop>(node)->condition->registerIndex, context.assembler->end());
			context.assembler->seek(context.assembler->end());
		}
		return;


		case WTNode::Branch: {
			compile(context, as<wt::Branch>(node)->condition);

			BytecodeAssembler::Label condJmp = context.assembler->createLabel();
			context.assembler->nope();

			compile(context, as<wt::Branch>(node)->thenBody);

			if(as<wt::Branch>(node)->elseBody) {
				BytecodeAssembler::Label elseJmp = context.assembler->createLabel();
				context.assembler->nope();

				context.assembler->seek(condJmp);
				context.assembler->jumpZ(as<wt::Loop>(node)->condition->registerIndex, context.assembler->end());
				context.assembler->seek(context.assembler->end());

				compile(context, as<wt::Branch>(node)->elseBody);

				context.assembler->seek(elseJmp);
				context.assembler->jump(context.assembler->end());
			} else {
				context.assembler->seek(condJmp);
				context.assembler->jumpZ(as<wt::Loop>(node)->condition->registerIndex, context.assembler->end());
			}
			context.assembler->seek(context.assembler->end());
		}
		return;


		case WTNode::Return:
			if(as<wt::Return>(node)->value->type == WTNode::Integer) {
				context.assembler->retI(as<wt::Int>(as<wt::Return>(node)->value)->value);
			} else {
				compile(context, as<wt::Return>(node)->value);
				context.assembler->ret(as<wt::Return>(node)->value->registerIndex);
			}
		return;


		default:
			throw CompilationErrorException("Unknown node type", node);
	}
}

void BytecodeCompiler::compile(Context &context, WTFunction *func) {
	BytecodeAssembler *ass = context.assembler;
	context.assembler = &context.externalAssemblers[func];

	context.assembler->function(func->index, func->stackSize, func->args.size());
	compile(context, func->body);
	context.assembler->exit();

	context.assembler = ass;
}

void BytecodeCompiler::compile(Context &context, WTExpression *node) {
	switch(node->type) {

		case WTNode::Add:
		case WTNode::Substract:
		case WTNode::Multiply:
		case WTNode::Divide:
		case WTNode::Equals:
		case WTNode::NotEquals:
		case WTNode::LessThan:
		case WTNode::GreaterThan:
			compile(context, as<wt::BinOp>(node));
		return;


		case WTNode::Assignation:
			compile(context, as<wt::Assignation>(node)->value);
			if(node->registerIndex != as<wt::Assignation>(node)->value->registerIndex) {
				context.assembler->copy(node->registerIndex, as<wt::Assignation>(node)->value->registerIndex);
			}
		return;

		case WTNode::Call:
			if(!context.externalAssemblers.exists(as<wt::Call>(node)->func)) {
				compile(context, as<wt::Call>(node)->func);
			}
			for(WTExpression *e : as<wt::Call>(node)->args) {
				compile(context, e);
				context.assembler->pushArg(e->registerIndex);
			}
			context.assembler->call(as<wt::Call>(node)->registerIndex, as<wt::Call>(node)->func->index);
		return;


		case WTNode::Integer:
			context.assembler->set(node->registerIndex, as<wt::Int>(node)->value);
		return;


		case WTNode::Variable:
		return;

		case WTNode::Cast:
			if(as<wt::Cast>(node)->expressionType == context.typeSystem->getFloatType() && as<wt::Cast>(node)->expression->expressionType == context.typeSystem->getIntType()) {
				compile(context, as<wt::Cast>(node)->expression);
				context.assembler->toFloat(as<wt::Cast>(node)->registerIndex, as<wt::Cast>(node)->expression->registerIndex);
			} else {
				throw CompilationErrorException("Invalid type cast", node);
			}
		break;


		default:
			throw CompilationErrorException("Unknown node type", node);
	}
}


void BytecodeCompiler::compile(Context &context, wt::BinOp *node) {
	compile(context, node->lhs);
	compile(context, node->rhs);
	uint to = node->registerIndex;
	uint l = node->lhs->registerIndex;
	uint r = node->rhs->registerIndex;
	bool isFloat = node->expressionType == context.typeSystem->getFloatType();
	switch(node->type) {
		case WTNode::Add:
			if(isFloat) {
				context.assembler->addF(to, l, r);
			} else {
				context.assembler->addI(to, l, r);
			}
		return;

		case WTNode::Substract:
			if(isFloat) {
				context.assembler->subF(to, l, r);
			} else {
				context.assembler->subI(to, l, r);
			}
		return;

		case WTNode::Multiply:
			if(isFloat) {
				context.assembler->mulF(to, l, r);
			} else {
				context.assembler->mulI(to, l, r);
			}
		return;

		case WTNode::Divide:
			if(isFloat) {
				context.assembler->divF(to, l, r);
			} else {
				context.assembler->divI(to, l, r);
			}
		return;

		case WTNode::Equals:
			context.assembler->equals(to, l, r);
		return;

		case WTNode::NotEquals:
			context.assembler->notEq(to, l, r);
		return;

		case WTNode::LessThan:
			if(isFloat) {
				context.assembler->lessF(to, l, r);
			} else {
				context.assembler->lessI(to, l, r);
			}
		return;

		case WTNode::GreaterThan:
			if(isFloat) {
				context.assembler->greaterF(to, l, r);
			} else {
				context.assembler->greaterI(to, l, r);
			}
		return;

		default:
			throw CompilationErrorException("Unknown node type", node);
	}
}

}
}
