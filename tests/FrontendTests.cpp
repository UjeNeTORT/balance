#include <gtest/gtest.h>

#include <fstream>
#include <sstream>
#include <string>

#include "AST/AstDumper.h"
#include "Frontend/Driver.h"

namespace
{

std::string dataPath(const std::string& name)
{
    return std::string(TEST_DATA_DIR) + "/" + name;
}

std::string dumpFile(const std::string& sourceName)
{
    Balance::Driver driver;

    int status = driver.parse(dataPath(sourceName));

    EXPECT_EQ(status, 0) << "parse failed for " << sourceName;

    std::stringstream result;

    Balance::AST::AstDumper dumper(result);

    driver.getCompUnit()->accept(dumper);

    return result.str();
}

std::string readAnswer(const std::string& answerName)
{
    std::ifstream answerFile(dataPath(answerName));

    EXPECT_TRUE(answerFile.is_open()) << "can't open " << answerName;

    std::stringstream answer;
    answer << answerFile.rdbuf();

    return answer.str();
}

void runTest(const std::string& name)
{
    EXPECT_EQ(dumpFile(name + ".sy"), readAnswer(name + ".ans"));
}

void runFailTest(const std::string& name)
{
    Balance::Driver driver;

    int status = 0;

    try
    {
        status = driver.parse(dataPath(name + ".sy"));
    }
    catch (std::exception&)
    {
        status = 1;
    }

    EXPECT_NE(status, 0) << name << " should fail to parse";
}

} // namespace

// ----- Declarations -----

TEST(Decl, VarSimple) { runTest("decl_var_simple"); }

TEST(Decl, ConstScalar) { runTest("decl_const_scalar"); }

TEST(Decl, Arrays) { runTest("decl_arrays"); }

TEST(Decl, InitLists) { runTest("decl_init_lists"); }

TEST(Decl, MultiDef) { runTest("decl_multi_def"); }

// ----- Functions -----

TEST(Func, VoidNoParams) { runTest("func_void_no_params"); }

TEST(Func, Params) { runTest("func_params"); }

TEST(Func, ArrayParams) { runTest("func_array_params"); }

TEST(Func, Calls) { runTest("func_calls"); }

// ----- Control flow -----

TEST(Stmt, IfElse) { runTest("stmt_if_else"); }

TEST(Stmt, DanglingElse) { runTest("stmt_dangling_else"); }

TEST(Stmt, While) { runTest("stmt_while"); }

TEST(Stmt, BreakContinue) { runTest("stmt_break_continue"); }

TEST(Stmt, NestedBlocks) { runTest("stmt_nested_blocks"); }

// ----- Expressions -----

TEST(Expr, Precedence) { runTest("expr_precedence"); }

TEST(Expr, Unary) { runTest("expr_unary"); }

TEST(Expr, Logical) { runTest("expr_logical"); }

TEST(Expr, ArrayIndexing) { runTest("expr_array_indexing"); }

// ----- Lexical -----

TEST(Lex, Numbers) { runTest("lex_numbers"); }

TEST(Lex, Comments) { runTest("lex_comments"); }

TEST(Lex, EmptyFile) { runTest("lex_empty_file"); }

// ----- Negative -----

TEST(Negative, MissingSemicolon) { runFailTest("neg_missing_semicolon"); }

TEST(Negative, BadCharacter) { runFailTest("neg_bad_character"); }

TEST(Negative, UnclosedBrace) { runFailTest("neg_unclosed_brace"); }

TEST(Negative, UnterminatedComment) { runFailTest("neg_unterminated_comment"); }

TEST(Negative, ConstWithoutInit) { runFailTest("neg_const_without_init"); }
