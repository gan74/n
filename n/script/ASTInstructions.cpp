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
#include "ASTInstructions.h"

namespace n {
namespace script {

WTInstruction *ASTDeclaration::toWorkTree(WTBuilder &builder) const {
	WTVariable *var = builder.declareVar(name, typeName, position);

	builder.enterScope();
	WTExpression *val = value ? value->toWorkTree(builder, var->registerIndex) : new WTInt(0, builder.getTypeSystem()->getType(typeName), var->registerIndex);
	builder.leaveScope();

	return new WTExprInstr(new WTAssignation(var, val));
}

WTInstruction *ASTBlock::toWorkTree(WTBuilder &builder) const {
	builder.enterScope();
	core::Array<WTInstruction *> in;
	for(ASTInstruction *i : instructions) {
		WTInstruction *ii = i->toWorkTree(builder);
		if(ii) {
			in.append(ii);
		}
	}
	builder.leaveScope();
	return new WTBlock(in);
}

WTInstruction *ASTLoop::toWorkTree(WTBuilder &builder) const {
	builder.enterScope();
	WTExpression *c = condition->toWorkTree(builder, builder.allocRegister());
	builder.leaveScope();

	return new WTLoop(c, body->toWorkTree(builder));
}

WTInstruction *ASTBranch::toWorkTree(WTBuilder &builder) const {
	builder.enterScope();
	WTExpression *c = condition->toWorkTree(builder, builder.allocRegister());
	builder.leaveScope();

	builder.enterScope();
	WTInstruction *t = thenBody->toWorkTree(builder);
	builder.leaveScope();

	builder.enterScope();
	WTInstruction *e = elseBody ? elseBody->toWorkTree(builder) : 0;
	builder.leaveScope();

	return new WTBranch(c, t, e);
}

WTInstruction *ASTExprInstruction::toWorkTree(WTBuilder &builder) const {
	builder.enterScope();
	N_SCOPE(builder.leaveScope());

	return new WTExprInstr(expression->toWorkTree(builder, builder.allocRegister()));
}

WTInstruction *ASTFunctionDeclaration::toWorkTree(WTBuilder &builder) const {
	WTFunction *function = builder.getFunc(name, position);
	builder.enterFunction(function);
	function->body = body->toWorkTree(builder);
	builder.leaveFunction();
	return 0;
}

static WTExpression *cast(WTExpression *expr, WTVariableType *type, uint reg) {
	return expr->expressionType == type ? expr : new WTCast(expr, type, reg);
}

static WTExpression *cast(WTExpression *expr, WTVariableType *type, uint reg, WTBuilder &builder, TokenPosition position) {
	if(!builder.getTypeSystem()->assign(type, expr->expressionType)) {
		throw ValidationErrorException("Assignation of incompatible types", position);
	}
	return cast(expr, type, reg);
}

WTInstruction *ASTReturn::toWorkTree(WTBuilder &builder) const {
	WTFunction *function = builder.getCurrentFunction();
	if(!function) {
		throw ValidationErrorException("return statement outside function", position);
	}
	#warning function may not have a return statement

	builder.enterScope();
	N_SCOPE(builder.leaveScope());

	uint reg = builder.allocRegister();
	return new WTReturn(cast(expression->toWorkTree(builder, reg), function->returnType, reg, builder, position));
}



void ASTDeclaration::lookupFunctions(WTBuilder &) const {
}

void ASTBlock::lookupFunctions(WTBuilder &builder) const {
	for(ASTInstruction *i : instructions) {
		i->lookupFunctions(builder);
	}
}

void ASTLoop::lookupFunctions(WTBuilder &builder) const {
	body->lookupFunctions(builder);
}

void ASTBranch::lookupFunctions(WTBuilder &builder) const {
	thenBody->lookupFunctions(builder);
	if(elseBody) {
		elseBody->lookupFunctions(builder);
	}
}

void ASTExprInstruction::lookupFunctions(WTBuilder &) const {
}

void ASTFunctionDeclaration::lookupFunctions(WTBuilder &builder) const {
	builder.enterFunction();
	core::Array<WTVariable *> arg;
	for(ASTDeclaration *d : args) {
		if(d->value) {
			throw ValidationErrorException("Function parameter \"" + d->name + "\" should not have a value", position);
		}
		arg.append(builder.declareVar(d->name, d->typeName, d->position));
	}
	builder.leaveFunction();

	WTVariableType *ret = builder.getTypeSystem()->getType(retTypeName);
	if(!ret) {
		throw ValidationErrorException("\"" + retTypeName + "\" was not declared in this scope", position);
	}

	builder.declareFunc(name, arg, ret, position);
}

void ASTReturn::lookupFunctions(WTBuilder &) const {
}

}
}
