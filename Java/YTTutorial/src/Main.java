
public class Main {
	public static void main(String[] args) {
		// single line comments
		/*
		 * multi line comments
		 */
		
		/*
		 * variable declarations
		 * int, double, float, char, byte, boolean, long
		 */
		
		// type name = value;
		int i = 1;
		i = 3; // don't have to re-state type
		
		/*
		 * expressions
		 */
		
		// arithmetic operators: +, -, *, /, %
		i = i + 3;
		
		// assignment operators: +=, -=, *=, /=, %=
		i += 3; // i = i + 3
		
		// post-in/decrement operators
		// returns current value then in/decrements
		i++; // i += 1; i = i + 1
		i--; // i -= 1
		
		i = 15;
		System.out.println(3 * i++); // "45", i = 16
		
		// pre-in/decrement operators
		// in/decrements then returns new value
		++i;
		--i;
		
		i = 15;
		System.out.println(3 * --i); // "42", i = 14
	}
}
