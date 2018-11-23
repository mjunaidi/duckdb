//===----------------------------------------------------------------------===//
//
//                         DuckDB
//
// parser/statement/transaction_statement.hpp
//
// Author: Mark Raasveldt
//
//===----------------------------------------------------------------------===//
#pragma once

#include <vector>

#include "parser/sql_node_visitor.hpp"
#include "parser/sql_statement.hpp"

#include "parser/expression.hpp"

namespace duckdb {

class TransactionStatement : public SQLStatement {
  public:
	TransactionStatement(TransactionType type)
	    : SQLStatement(StatementType::TRANSACTION), type(type){};
	virtual ~TransactionStatement() {
	}

	virtual std::string ToString() const {
		return "Transaction";
	}
	virtual std::unique_ptr<SQLStatement> Accept(SQLNodeVisitor *v) {
		return v->Visit(*this);
	}

	virtual bool Equals(const SQLStatement *other_) {
		if (!SQLStatement::Equals(other_)) {
			return false;
		}
		throw NotImplementedException("Equality not implemented!");
	}

	TransactionType type;
};
} // namespace duckdb
