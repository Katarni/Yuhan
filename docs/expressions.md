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

\<expInt13> ::= \<expInt12> {\<opInt13> \<expInt12>} | \<expChar13>  
\<expFloat13> ::= \<expFloat12> {\<opFloat13>  \<expFloat12>} | \<expInt13>  
\<expString13> ::= \<expString4> {\<opString13> \<expStringOrChar12>}  
\<expArray13> ::= \<expArray1> {\<opArray13> \<expArray1>}  
\<expBool13> ::= \<expBool12> {\<opInt13> \<expBool12>}   
\<expChar13> ::= \<expChar12> {\<opInt13> \<expChar12>} | \<expBool13>  
\<opInt13> ::= \<opFloat13> | `%=` | `&=` | `|=` | `^=` | `<<=` | `>>=`  
\<opFloat13> ::= \<opString13> | `-=` | `*=` | `/=`  
\<opString13> ::= \<opArray13> | `+=`  
\<opArray13> ::= `=`  

\<expStringOrChar12> ::= \<expString4> | \<expChar12>  
\<expFloat12> ::= \<expFloat11> | \<expInt12>
\<expInt12> ::= \<expInt11> | \<expChar12>  
\<expChar12> ::= \<expChar11> | \<expBool12>  
\<expBool12> ::= \<expBoolOrCharOrInt11> {`or` \<expBoolOrCharOrInt11>}  

\<expBoolOrCharOrInt11> ::=  \<expBool11> |  \<expChar11> |  \<expInt11>  
\<expFloat11> ::= \<expFloat10>  |  \<expInt11>
\<expInt11> ::= \<expInt10> | \<expChar11>  
\<expChar11> ::= \<expChar10> | \<expBool11>  
\<expBool11> ::= \<expBoolOrCharOrInt10> {`and` \<expBoolOrCharOrInt10>}  

\<expBoolOrCharOrInt10> ::=  \<expBool10> |  \<expChar10> |  \<expInt10>  
\<expFloat10> ::= \<expFloat9> | \<expInt10>  
\<expInt10> ::= \<expInt9> {`|` \<expInt9>} | \<expChar10>  
\<expChar10> ::= \<expChar9> {`|` \<expChar9>} | \<expBool10>  
\<expBool10> ::= \<expBool9> {`|` \<expBool9>}

\<expFloat9> ::= \<expFloat8> | \<expInt9>  
\<expInt9> ::= \<expInt8> {`^` \<expInt8>} | \<expChar9>  
\<expChar9> ::= \<expChar8> {`^` \<expChar8>} | \<expBool9>  
\<expBool9> ::= \<expBool8> {`^` \<expBool8>}  

\<expFloat8> ::= \<expFloat7> | \<expInt8>  
\<expInt8> ::= \<expInt7> {`&` \<expInt7>} | \<expChar8>  
\<expChar8> ::= \<expChar7> {`&` \<expChar7>} | \<expBool8>  
\<expBool8> ::= \<expBool7> {`&` \<expBool7>}  

\<expBool7> ::= \<expChar6> \<op7> \<expChar6> | \<expInt6> \<op7> \<expInt6> | \<expString4> \<op7> \<expString4> | \<expArray1> \<op7> \<expArray1> | \<expBool6>  {\<op7> \<expBool6>}  
\<op7> ::= `==` | `!=`  
\<expFloat7> ::= \<expFloat6> | \<expInt7>  
\<expInt7> ::= \<expInt6> | \<expChar7>  
\<expChar7> ::= \<expChar6> | \<expBool7>

\<expBool6> ::= \<expChar2> \<op6> \<expChar2> | \<expInt5> \<op6> \<expInt5> | \<expString4> \<op6> \<expString4> | \<expArray1> \<op6> \<expArray1> | \<expBool2>  {\<op6> \<expBool2>}  
\<op6> ::= `>=` | `<=` | `>` | `<`   
\<expFloat6> ::= \<expFloat5> | \<expInt6>  
\<expInt6> ::= \<expInt5> | \<expChar6>  
\<expChar6> ::= \<expChar2> | \<expBool6>  

\<expFloat5> ::= \<expFloat4> | \<expInt5>  
\<expInt5> ::= \<expInt4> {\<op5> \<expInt4>}  
\<op5> ::= `<<` | `>>`  


\<expString4> ::= \<expString1> {`+` \<expString1>}  
\<expFloat4> ::= \<expFloat3> {\<op4> \<expFloat3>} | \<expInt4>  
\<expInt4> ::= \<expInt3> {\<op4> \<expInt3>}  
\<op4> ::= `+` | `-`  


\<expBool2> ::= `!` \<expBool1> | `+` \<expBool1>  
\<expChar2> ::= `!` \<expChar1> | `++` \<expChar1> | `--` \<expChar1> | \<expBool2>  


\<expArray1> ::= `(`\<expArray14>`)` | \<expArray0>  
\<expString1> ::= `(`\<expString14>`)` | \<expString0>  
\<expBool1> ::= `(`\<expBool14>`)` | \<expBool0>  
\<expChar1> ::= `(`\<expChar14>`)` | \<expChar0> | \<expBool1>  


\<expArray0> ::= \<identifierArray>  
\<expString0> ::= \<identifierString> | \<literalString>  
\<expBool0> ::= \<identifierBool> | \<literalBool>  
\<expChar0> ::= \<identifierChar> | \<literalChar> | \<expBool0>  
