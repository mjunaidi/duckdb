//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/planner/tableref/bound_table_function.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/planner/bound_tableref.hpp"
#include "duckdb/planner/logical_operator.hpp"

namespace duckdb {

//! Represents a reference to a table-producing function call
class BoundTableFunction : public BoundTableRef {
public:
	BoundTableFunction(unique_ptr<LogicalOperator> get)
	    : BoundTableRef(TableReferenceType::TABLE_FUNCTION), get(move(get)) {
	}

	unique_ptr<LogicalOperator> get;
};

} // namespace duckdb
