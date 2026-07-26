%skeleton "lalr1.cc" // -*- C++ -*-
%require "3.8.2"
%header

%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error detailed
%define parse.lac full

%locations

%code requires
{
    #include <string>
    #include <vector>

    #include "AST/Node.h"

    namespace Balance
    {
        class Driver;
    }
}

%param { Balance::Driver& drv }

%code
{
    #include "Frontend/Driver.h"

    namespace AST = Balance::AST;
}

%define api.token.prefix {TOK_}
%token
    CONST       "const"
    INT         "int"
    FLOAT       "float"
    VOID        "void"
    IF          "if"
    ELSE        "else"
    WHILE       "while"
    BREAK       "break"
    CONTINUE    "continue"
    RETURN      "return"
    PLUS        "+"
    MINUS       "-"
    STAR        "*"
    SLASH       "/"
    PERCENT     "%"
    ASSIGN      "="
    EQUAL       "=="
    NOT_EQUAL   "!="
    LESS        "<"
    GREATER     ">"
    LESS_EQ     "<="
    GREATER_EQ  ">="
    AND         "&&"
    OR          "||"
    NOT         "!"
    LPAREN      "("
    RPAREN      ")"
    LBRACKET    "["
    RBRACKET    "]"
    LBRACE      "{"
    RBRACE      "}"
    COMMA       ","
    SEMICOLON   ";"
;

%token <std::string>    ID          "identifier"
%token <int>            INT_CONST   "integer constant"
%token <float>          FLOAT_CONST "float constant"

%nterm <std::vector<Balance::AST::NodePtr>>         CompUnitItems
%nterm <Balance::AST::VarDeclPtr>                   Decl
%nterm <Balance::AST::VarDeclPtr>                   ConstDecl
%nterm <Balance::AST::VarDeclPtr>                   VarDecl
%nterm <Balance::AST::BaseType>                     BType
%nterm <std::vector<Balance::AST::VarDefPtr>>       ConstDefList
%nterm <std::vector<Balance::AST::VarDefPtr>>       VarDefList
%nterm <Balance::AST::VarDefPtr>                    ConstDef
%nterm <Balance::AST::VarDefPtr>                    VarDef
%nterm <std::vector<Balance::AST::ExprPtr>>         ArrayDims
%nterm <Balance::AST::InitValPtr>                   InitVal
%nterm <std::vector<Balance::AST::InitValPtr>>      InitValList
%nterm <Balance::AST::FuncDefPtr>                   FuncDef
%nterm <std::vector<Balance::AST::FuncParamPtr>>    FuncFParamsOpt
%nterm <std::vector<Balance::AST::FuncParamPtr>>    FuncFParams
%nterm <Balance::AST::FuncParamPtr>                 FuncFParam
%nterm <std::vector<Balance::AST::ExprPtr>>         ParamDims
%nterm <Balance::AST::BlockPtr>                     Block
%nterm <std::vector<Balance::AST::StmtPtr>>         BlockItems
%nterm <Balance::AST::StmtPtr>                      BlockItem
%nterm <Balance::AST::StmtPtr>                      Stmt
%nterm <Balance::AST::ExprPtr>                      Exp
%nterm <Balance::AST::LValPtr>                      LVal
%nterm <std::vector<Balance::AST::ExprPtr>>         LValIndices
%nterm <std::vector<Balance::AST::ExprPtr>>         FuncRParams

%precedence THEN
%precedence "else"

%left "||"
%left "&&"
%left "==" "!="
%left "<" ">" "<=" ">="
%left "+" "-"
%left "*" "/" "%"
%precedence UNARY

%%

CompUnit:   CompUnitItems YYEOF
            {
                drv.formCompUnit(std::move($1));
            }
        ;

CompUnitItems:  %empty
                {
                }
            |   CompUnitItems Decl
                {
                    $$ = std::move($1);
                    $$.push_back($2);
                }
            |   CompUnitItems FuncDef
                {
                    $$ = std::move($1);
                    $$.push_back($2);
                }
            ;

// ----- Declarations -----

Decl:   ConstDecl
        {
            $$ = $1;
        }
    |   VarDecl
        {
            $$ = $1;
        }
    ;

ConstDecl:  "const" BType ConstDefList ";"
            {
                $$ = drv.construct<AST::VarDeclNode>($2, std::move($3), true);
            }
        ;

VarDecl:    BType VarDefList ";"
            {
                $$ = drv.construct<AST::VarDeclNode>($1, std::move($2), false);
            }
        ;

BType:  "int"
        {
            $$ = AST::BaseType::Int;
        }
    |   "float"
        {
            $$ = AST::BaseType::Float;
        }
    ;

ConstDefList:   ConstDef
                {
                    $$.push_back($1);
                }
            |   ConstDefList "," ConstDef
                {
                    $$ = std::move($1);
                    $$.push_back($3);
                }
            ;

VarDefList:     VarDef
                {
                    $$.push_back($1);
                }
            |   VarDefList "," VarDef
                {
                    $$ = std::move($1);
                    $$.push_back($3);
                }
            ;

ConstDef:   ID ArrayDims "=" InitVal
            {
                $$ = drv.construct<AST::VarDefNode>(std::move($1),
                                                    std::move($2), $4);
            }
        ;

VarDef:     ID ArrayDims
            {
                $$ = drv.construct<AST::VarDefNode>(std::move($1),
                                                    std::move($2));
            }
        |   ID ArrayDims "=" InitVal
            {
                $$ = drv.construct<AST::VarDefNode>(std::move($1),
                                                    std::move($2), $4);
            }
        ;

ArrayDims:  %empty
            {
            }
        |   ArrayDims "[" Exp "]"
            {
                $$ = std::move($1);
                $$.push_back($3);
            }
        ;

InitVal:    Exp
            {
                $$ = drv.construct<AST::InitValNode>($1);
            }
        |   "{" "}"
            {
                $$ = drv.construct<AST::InitValNode>(
                    std::vector<AST::InitValPtr>{});
            }
        |   "{" InitValList "}"
            {
                $$ = drv.construct<AST::InitValNode>(std::move($2));
            }
        ;

InitValList:    InitVal
                {
                    $$.push_back($1);
                }
            |   InitValList "," InitVal
                {
                    $$ = std::move($1);
                    $$.push_back($3);
                }
            ;

// ----- Functions -----

FuncDef:    BType ID "(" FuncFParamsOpt ")" Block
            {
                $$ = drv.construct<AST::FuncDefNode>($1, std::move($2),
                                                     std::move($4), $6);
            }
        |   "void" ID "(" FuncFParamsOpt ")" Block
            {
                $$ = drv.construct<AST::FuncDefNode>(AST::BaseType::Void,
                                                     std::move($2),
                                                     std::move($4), $6);
            }
        ;

FuncFParamsOpt: %empty
                {
                }
            |   FuncFParams
                {
                    $$ = std::move($1);
                }
            ;

FuncFParams:    FuncFParam
                {
                    $$.push_back($1);
                }
            |   FuncFParams "," FuncFParam
                {
                    $$ = std::move($1);
                    $$.push_back($3);
                }
            ;

FuncFParam:     BType ID
                {
                    $$ = drv.construct<AST::FuncParamNode>($1, std::move($2));
                }
            |   BType ID "[" "]" ParamDims
                {
                    $$ = drv.construct<AST::FuncParamNode>($1, std::move($2),
                                                           std::move($5));
                }
            ;

ParamDims:  %empty
            {
            }
        |   ParamDims "[" Exp "]"
            {
                $$ = std::move($1);
                $$.push_back($3);
            }
        ;

// ----- Statements -----

Block:  "{" BlockItems "}"
        {
            $$ = drv.construct<AST::BlockNode>(std::move($2));
        }
    ;

BlockItems: %empty
            {
            }
        |   BlockItems BlockItem
            {
                $$ = std::move($1);
                $$.push_back($2);
            }
        ;

BlockItem:  Decl
            {
                $$ = $1;
            }
        |   Stmt
            {
                $$ = $1;
            }
        ;

Stmt:   LVal "=" Exp ";"
        {
            $$ = drv.construct<AST::AssignNode>($1, $3);
        }
    |   ";"
        {
            $$ = drv.construct<AST::ExprStmtNode>();
        }
    |   Exp ";"
        {
            $$ = drv.construct<AST::ExprStmtNode>($1);
        }
    |   Block
        {
            $$ = $1;
        }
    |   "if" "(" Exp ")" Stmt %prec THEN
        {
            $$ = drv.construct<AST::IfNode>($3, $5);
        }
    |   "if" "(" Exp ")" Stmt "else" Stmt
        {
            $$ = drv.construct<AST::IfNode>($3, $5, $7);
        }
    |   "while" "(" Exp ")" Stmt
        {
            $$ = drv.construct<AST::WhileNode>($3, $5);
        }
    |   "break" ";"
        {
            $$ = drv.construct<AST::BreakNode>();
        }
    |   "continue" ";"
        {
            $$ = drv.construct<AST::ContinueNode>();
        }
    |   "return" ";"
        {
            $$ = drv.construct<AST::ReturnNode>();
        }
    |   "return" Exp ";"
        {
            $$ = drv.construct<AST::ReturnNode>($2);
        }
    ;

// ----- Expressions -----

Exp:    Exp "+" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Add, $3);
        }
    |   Exp "-" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Sub, $3);
        }
    |   Exp "*" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Mul, $3);
        }
    |   Exp "/" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Div, $3);
        }
    |   Exp "%" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Mod, $3);
        }
    |   Exp "<" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Less, $3);
        }
    |   Exp ">" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Greater,
                                                  $3);
        }
    |   Exp "<=" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::LessEqual,
                                                  $3);
        }
    |   Exp ">=" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1,
                                                  AST::BinaryOp::GreaterEqual,
                                                  $3);
        }
    |   Exp "==" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::Equal,
                                                  $3);
        }
    |   Exp "!=" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::NotEqual,
                                                  $3);
        }
    |   Exp "&&" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1,
                                                  AST::BinaryOp::LogicalAnd,
                                                  $3);
        }
    |   Exp "||" Exp
        {
            $$ = drv.construct<AST::BinaryOpNode>($1, AST::BinaryOp::LogicalOr,
                                                  $3);
        }
    |   "+" Exp %prec UNARY
        {
            $$ = drv.construct<AST::UnaryOpNode>($2, AST::UnaryOp::Plus);
        }
    |   "-" Exp %prec UNARY
        {
            $$ = drv.construct<AST::UnaryOpNode>($2, AST::UnaryOp::Minus);
        }
    |   "!" Exp %prec UNARY
        {
            $$ = drv.construct<AST::UnaryOpNode>($2, AST::UnaryOp::LogicalNot);
        }
    |   "(" Exp ")"
        {
            $$ = $2;
        }
    |   LVal
        {
            $$ = $1;
        }
    |   INT_CONST
        {
            $$ = drv.construct<AST::IntLiteralNode>($1);
        }
    |   FLOAT_CONST
        {
            $$ = drv.construct<AST::FloatLiteralNode>($1);
        }
    |   ID "(" ")"
        {
            $$ = drv.construct<AST::CallNode>(std::move($1));
        }
    |   ID "(" FuncRParams ")"
        {
            $$ = drv.construct<AST::CallNode>(std::move($1), std::move($3));
        }
    ;

LVal:   ID LValIndices
        {
            $$ = drv.construct<AST::LValNode>(std::move($1), std::move($2));
        }
    ;

LValIndices:    %empty
                {
                }
            |   LValIndices "[" Exp "]"
                {
                    $$ = std::move($1);
                    $$.push_back($3);
                }
            ;

FuncRParams:    Exp
                {
                    $$.push_back($1);
                }
            |   FuncRParams "," Exp
                {
                    $$ = std::move($1);
                    $$.push_back($3);
                }
            ;

%%

void yy::parser::error(const location_type& loc, const std::string& msg)
{
    std::cerr << loc << ": " << msg << '\n';
}
