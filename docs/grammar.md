
\<program> ::= \<includes> \<namespaces> \<programBody>  
\<includes> ::= {`#include` \<literalString>}  
\<namespaces> ::= {`using` \<namespace> `;`}
\<namespace> ::= \<identifier> {`.` \<identifier>}  
\<programBody> ::= {(\<function> | \<varDefinition>) | \<struct>)}  
\<struct> ::= `struct` \<identifier> `{` \<programBody> `}` `;`  
\<type> ::= \<identifier>  
\<varDefinition> ::= \<type> \<var> {`,`  \<var>}`;`  
\<var> ::= \<identifier> | \<identifier> `=` \<exp12>  
\<funcVarDefinition> ::= \<type> \<identifier> {`,` \<type> \<identifier>} {`,` \<type> \<identifier> `=` \<exp12>} | \[\<type> \<identifier> `=` \<exp12> {`,` \<type> \<identifier> `=` \<exp12>}]  
\<function> ::= \<type> \<identifier> `(` \<funcVarDefinition> `)` \<block>  
\<block> ::= `{`\<statement>`}`  
\<statement> ::= \<if> | \<while> | \<for> | \<switch> | `break` `;` | `continue` `;` | \<varDefenition> | \<return> | \<block> | `;`  
\<if> ::= `if` `(` \<exp14> `)` \<block> \[`else` \<block>]  
\<while> ::= `while` `(` \<exp14> `)` \<block>  
\<for> ::= `for` `(` \[\<exp14>] `;` \[\<exp14>] `;` \[\<exp14>] `)` \<block>  
\<switchItem> ::= `case` \<literal> `:` \<block>  
\<switch> ::= `switch` `(` \<exp14> `)` `{` {\<switchItem>} \[`default` `:` \<block>] `}`  
\<return> ::= `return` \[\<exp14>] `;`  
