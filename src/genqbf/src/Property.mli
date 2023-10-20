module E=Expression
  
type property =
| G   of E.expression
| F   of E.expression
| X   of E.expression
| U   of E.expression * E.expression
| R   of E.expression * E.expression
| Neg of property
| Or  of property * property
| And of property * property
| Implies of property * property
| Iff of property * property

    
