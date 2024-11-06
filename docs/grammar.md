
\<program> ::= \<includes> \<namespaces> {\<programBody>} \<endOfFile>  
\<includes> ::= {`#include` \<literalString>}  
\<namespaces> ::= {`using` \<namespace> `;`}  
\<namespace> ::= \<identifier> {`.` \<identifier>}  
\<programBody> ::= (\<function> | \<varDefinition>) | \<struct>)  
\<struct> ::= `struct` \<identifier> `{` {\<programBody>} `}` `;`  
\<type> ::= `int` | `float` | `bool` | `char` | `string` | \[`typename`] `array` `<` \<type>`,` \<literalInt> `>` | `typename` \<identifier> {`::` \<identifier>}  
\<varDefinition> ::= \<type> \<var> {`,`  \<var>}`;`  
\<var> ::= \<identifier> | \<identifier> `=` \<exp12>  
\<funcVarDefinition> ::= \<type> \<identifier> {`,` \<type> \<identifier>}  
\<function> ::= `func` \<type> \<identifier> `(` \[\<funcVarDefinition>] `)` \<block>  
\<block> ::= `{`{\<statement>}`}`  
\<statement> ::= \<if> | \<while> | \<for> | \<switch> | `break` `;` | `continue` `;` | \<varDefenition> | \<return> | \<block> | `;` | \<exp14>  
\<if> ::= `if` `(` \<exp14> `)` \<statement> \[`else` \<statement>]  
\<while> ::= `while` `(` \<exp14> `)` \<statement>  
\<for> ::= `for` `(` \[(\<exp14> | \<varDefinition>)] `;` \[\<exp14>] `;` \[\<exp14>] `)` \<statement>  
\<switchItem> ::= `case` \<literal> \<block>  
\<switch> ::= `switch` `(` \<exp14> `)` `{` {\<switchItem>} \[`default` \<block>] `}`  
\<return> ::= `return` \[\<exp14>] `;`  
\<literal> ::= \<literalInt> | \<literalFloat> | \<literalExponential> | \<literalChar> | \<literalBool> | \<literalString>  