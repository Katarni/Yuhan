# Грамматика выражений

\<exp14> ::= \<exp13>{`,` \<exp13>}
\<exp13> ::= \<exp12>{\<op13> \<exp12>}
\<op13> ::= `=` | `+=` | `-=` | `*=` | `/=` | `%=` | `&=` | `|=` | `^=` | `<<=` | `>>=`
\<exp12> ::= \<exp11>{`or` \<exp12>}
\<exp11> ::= \<exp10>{`and` \<exp10>}
\<exp10> ::= \<exp9>{`|` \<exp9>}
\<exp9> ::= \<exp8>{`^` \<exp8>}
\<exp8> ::= \<exp7>{`&` \<exp7>}
\<exp7> ::= \<exp6>{\<op7> \<exp6>}
\<op7> ::= `==` | `!=`
\<exp6> ::= \<exp5>{\<op6> \<exp5>}
\<op6> ::= `>` | `<` | `<=` | `>=`
\<exp5> ::= \<exp4>{\<op5> \<exp4>}
\<op5> ::= `>>` | `<<`
\<exp4> ::= \<exp3>{\<op4> \<exp3>}
\<op4> ::= `+` | `-`
\<exp3> ::= \<exp2>{\<op3> \<exp2>}
\<op3> ::= `*` | `/` | `%`
\<exp2> ::= \<op2> \<exp1> | \<exp1>
\<op2> ::= `++` | `--` | `!` | `+` | `-`
\<exp1> ::= `(`\<exp14>`)` | `[`\<exp14>`]` |  \<>`.`\<identifier> | \<exp0>
\<exp0> ::= \<identifier> | \<numeric literal> | \<string literal> | \<exponential literal> | \<float literal>
