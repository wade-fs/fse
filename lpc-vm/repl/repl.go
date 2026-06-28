package repl

import (
	"bufio"
	"fmt"
	"io"

	"mudscript/lpc-vm/evaluator"
	"mudscript/lpc-vm/lexer"
	"mudscript/lpc-vm/object"
	"mudscript/lpc-vm/parser"
)

const prompt = ">> "

// Start starts MudScript REPL.
func Start(in io.Reader, out io.Writer) {
	scanner := bufio.NewScanner(in)
	env := object.NewEnvironment()

	for {
		fmt.Print(prompt)
		if !scanner.Scan() {
			return
		}

		line := scanner.Text()
		l := lexer.New(line)
		p := parser.New(l)

		program := p.ParseProgram()
		if len(p.Errors()) != 0 {
			printParserErrors(out, p.Errors())
			continue
		}

		// Evaluate AST
		evaluated := evaluator.Eval(program, env)
		if evaluated == nil {
			continue
		}

		io.WriteString(out, evaluated.Inspect())
		io.WriteString(out, "\n")
	}
}

func printParserErrors(out io.Writer, errors []string) {
	for _, msg := range errors {
		io.WriteString(out, msg)
		io.WriteString(out, "\n")
	}
}
