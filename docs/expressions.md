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
\<exp1> ::= `(`\<exp14>`)` | `[`\<exp14>`]`  | \<exp0>
\<exp0> ::= \<identifier> | \<numeric literal> | \<string literal> | \<exponential literal> | \<float literal>


\<exp14> ::=  \<expInt14> | \<expFloat14> | \<expString14> | \<expArray14> | \<expBool14>
\<expChar14> ::= {\<exp14> `,`} \<expChar13>
\<expBool14> ::= {\<exp14> `,`} \<expBool13>
\<expInt14> ::= {\<exp14> `,`} \<expInt13>
\<expFloat14> ::= {\<exp14> `,`} \<expFloat13>
\<expString14> ::= {\<exp14> `,`} \<expString13>
\<expArray14> ::= {\<exp14> `,`} \<expArray13>

\<expInt13> ::= \<expInt12> {\<opInt13> \<expInt12>} | \<expBool13> | \<expChar13>
\<expFloat13> ::= \<expFloat12> {\<opFloat13>  \<expFloat12>} | \<expInt13>
\<expString13> ::= \<expString12> {\<opString13> \<expStringOrChar12>}
\<expArray13> ::= \<expArray12> {\<opArray13> \<expArray12>}
\<expBool13> ::= \<expBool12> {\<opInt13> \<expBool12>} 
\<expChar13> ::= \<expChar12> {\<opInt13> \<expChar13>}
\<opInt13> ::= \<opFloat13> | `%=` | `&=` | `|=` | `^=` | `<<=` | `>>=`
\<opFloat13> ::= \<opString13> | `-=` | `*=` | `/=`
\<opString13> ::= \<opArray13> | `+=`
\<opArray13> ::= `=`

\<expStringOrChar12> ::= \<expString12> | \<expChar12>


