$$
\begin{align}

[\text{prog}] &\to [\text{Stmt}]^*\\ 
[\text{Stmt}] &\to 
\begin{cases}
    exit([\text{Expr}]);\\
    var\space\text{ident} = [\text{Expr}];\\
    \text{ident} = [\text{Expr}];\\
    \text{if}([\text{Expr}]) \\ 
    [\text{Scope}]
\end{cases}
\\
[\text{Scope}] &\to \{[\text{Stmt}]^*\}\\ 
[\text{IfPred}] &\to 
\begin{cases}
    \text{elif}(\text{[Expr]})[\text{Scope}][\text{IfPred}]\\
    \text{[Else]}\\
    \epsilon
\end{cases}\\
[\text{Else}] &\to 
\begin{cases}
    \text{else}(\text{[Expr]})[\text{Scope}]\\
    \epsilon
\end{cases}\\
[\text{Expr}] &\to 
\begin{cases}
    [\text{Term}]\\
    [\text{BinExpr}]
\end{cases}\\

[\text{BinExpr}] &\to
\begin{cases}
    [\text{Expr}] * [\text{Expr}] & \text{prec} = 1\\
    [\text{Expr}] / [\text{Expr}] & \text{prec} = 1\\
    [\text{Expr}] + [\text{Expr}] & \text{prec} = 0\\
    [\text{Expr}] - [\text{Expr}] & \text{prec} = 0\\
\end{cases}\\

[\text{Term}] &\to 
\begin{cases}
    \text{int\_lit}\\
    \text{ident}\\
    [\text{Expr}]\\
\end{cases}\\

\end{align}
$$