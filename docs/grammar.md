
\<program> ::= \<includes> \<namespaces> \<programBody>  
\<includes> ::= {`#include` \<literalString>}  
\<namespaces> ::= {`using` \<namespace> `;`}
\<namespace> ::= \<identifier> {`.` \<identifier>}  
\<programBody> ::= {(\<function> | \<varDefinition>) | \<struct>)}  
\<struct> ::= `struct` \<identifier> `{` \<programBody> `}` `;`  
\<type> ::= \<serviceWord>  
\<varDefinition> ::= \<type> \<var> {`,`  \<var>}`;`  
\<var> ::= \<identifier> | \<identifier> `=` \<exp12>  
\<funcVarDefinition> ::= \<type> \<identifier> {`,` \<type> \<identifier>} {`,` \<type> \<identifier> `=` \<exp12>} | \[\<type> \<identifier> `=` \<exp12> {`,` \<type> \<identifier> `=` \<exp12>}]  
\<function> ::= \<type> \<identifier> `(` \<funcVarDefinition> `)` \<block>  
\<block> ::= `{`\<statement>`}`  
\<statement> ::= \<if> | \<while> | \<for> | \<switch> | `break` `;` | `continue` `;` | \<varDefenition> | \<return> | \<block> | `;`  
\<if> ::= `if` `(` \<exp14> `)` \<statement> \[`else` \<statement>]  
\<while> ::= `while` `(` \<exp14> `)` \<statement>  
\<for> ::= `for` `(` \[\<exp14>] `;` \[\<exp14>] `;` \[\<exp14>] `)` \<statement>  
\<switchItem> ::= `case` \<literal> `:` \<statement>  
\<switch> ::= `switch` `(` \<exp14> `)` `{` {\<switchItem>} \[`default` `:` \<statement>] `}`  
\<return> ::= `return` \[\<exp14>] `;`  
