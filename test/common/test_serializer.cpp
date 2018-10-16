
#include "catch.hpp"

#include "expression_helper.hpp"

#include "common/serializer.hpp"
#include "common/types/data_chunk.hpp"

using namespace duckdb;
using namespace std;

TEST_CASE("Basic serializer test", "[serializer]") {
	Serializer serializer(4);
	serializer.Write<sel_t>(33);
	serializer.Write<uint64_t>(42);
	auto data = serializer.GetData();

	Deserializer source(data.data.get(), data.size);
	bool failed = false;
	REQUIRE(source.Read<sel_t>(failed) == 33);
	REQUIRE(source.Read<uint64_t>(failed) == 42);
	REQUIRE(!failed);
}

TEST_CASE("Data Chunk serialization", "[serializer]") {
	Value a = Value::INTEGER(33);
	Value b = Value::INTEGER(42);
	Value c = Value("hello");
	Value d = Value("world");
	// test serializing of DataChunk
	DataChunk chunk;
	vector<TypeId> types = {TypeId::INTEGER, TypeId::VARCHAR};
	chunk.Initialize(types);
	chunk.data[0].count = 2;
	chunk.data[0].SetValue(0, a);
	chunk.data[0].SetValue(1, b);
	chunk.data[1].count = 2;
	chunk.data[1].SetValue(0, c);
	chunk.data[1].SetValue(1, d);
	chunk.count = 2;

	Serializer serializer;
	chunk.Serialize(serializer);

	auto data = serializer.GetData();
	Deserializer source(data.data.get(), data.size);

	DataChunk other_chunk;
	REQUIRE(other_chunk.Deserialize(source));
	REQUIRE(other_chunk.count == 2);
	REQUIRE(other_chunk.column_count == 2);
	REQUIRE(other_chunk.data[0].count == 2);
	REQUIRE(Value::Equals(other_chunk.data[0].GetValue(0), a));
	REQUIRE(Value::Equals(other_chunk.data[0].GetValue(1), b));
	REQUIRE(other_chunk.data[1].count == 2);
	REQUIRE(Value::Equals(other_chunk.data[1].GetValue(0), c));
	REQUIRE(Value::Equals(other_chunk.data[1].GetValue(1), d));
}

TEST_CASE("Value serialization", "[serializer]") {
	Value a = Value::BOOLEAN(12);
	Value b = Value::TINYINT(12);
	Value c = Value::SMALLINT(12);
	Value d = Value::INTEGER(12);
	Value e = Value::BIGINT(12);
	Value f = Value::POINTER(12);
	Value g = Value::DATE(12);
	Value h = Value();
	Value i = Value("hello world");

	Serializer serializer(4);
	a.Serialize(serializer);
	b.Serialize(serializer);
	c.Serialize(serializer);
	d.Serialize(serializer);
	e.Serialize(serializer);
	f.Serialize(serializer);
	g.Serialize(serializer);
	h.Serialize(serializer);
	i.Serialize(serializer);

	auto data = serializer.GetData();
	Deserializer source(data.data.get(), data.size);

	Value a1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(a, a1));
	Value b1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(b, b1));
	Value c1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(c, c1));
	Value d1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(d, d1));
	Value e1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(e, e1));
	Value f1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(f, f1));
	Value g1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(g, g1));
	Value h1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(h, h1));
	Value i1 = Value::Deserialize(source);
	REQUIRE(Value::Equals(i, i1));
}

TEST_CASE("Expression serializer", "[serializer]") {
	{
		// operator, columnref, constant
		auto expression = ParseExpression("a + 2");
		REQUIRE(expression.get());

		Serializer serializer;
		expression->Serialize(serializer);

		auto data = serializer.GetData();
		Deserializer source(data.data.get(), data.size);
		auto deserialized_expression = Expression::Deserialize(source);
		REQUIRE(deserialized_expression.get());
		REQUIRE(expression->Equals(deserialized_expression.get()));
	}

	{
		// case, columnref, comparison, cast, conjunction
		auto expression =
		    ParseExpression("cast(a >= 2 as integer) OR CASE WHEN a >= b THEN "
		                    "a >= 5 ELSE a >= 7 END");
		REQUIRE(expression.get());

		Serializer serializer;
		expression->Serialize(serializer);

		auto data = serializer.GetData();
		Deserializer source(data.data.get(), data.size);
		auto deserialized_expression = Expression::Deserialize(source);
		REQUIRE(deserialized_expression.get());
		REQUIRE(expression->Equals(deserialized_expression.get()));
	}
}