#ifndef STATES_HPP
#define STATES_HPP

namespace States {
	/*
		states = 10001101
	*/

	// check state
	template<typename T>
	bool isActive(T* states, int target) {
		/*
			target = 3
			1 = 00000001
			1 << target = 00001000

			10001101
			00001000 &
			--------
			00001000 == 1 << target
		*/

		return (*states & (1 << target)) == (1 << target);
	}

	// activate state
	template<typename T>
	void activate(T* states, int target) {
		/*
			target = 4
			1 = 00000001
			1 << target = 00010000

			10001101
			00010000 |
			--------
			10011101
		*/

		*states |= 1 << target;
	}

	// uniquely activate state (no others can be active)
	template<typename T>
	void uniquelyActivate(T* states, int target) {
		/*
			target = 4
			1 = 00000001
			1 << target = 00010000

			10000000 (AS)
			00010000 |
			--------
			10010000
			00010000 &
			--------
			00010000
		*/

		activate<T>(states, target); // activate state first
		*states &= 1 << target; // deactivate others
	}

	// deactivate states
	template<typename T>
	void deactivate(T* states, int target) {
		/*
			target = 4
			1 = 00000001
			1 << target = 00010000
			~(1 << target) = 11101111

			10011101
			11101111 &
			--------
			10001101
		*/

		*states &= ~(1 << target);
	}

	// toggle state
	template<typename T>
	void toggle(T* states, int target) {
		/*
			target = 4
			1 = 00000001
			1 << target = 00010000

			10001101
			00010000 ^
			--------
			10011101
		*/

		*states ^= 1 << target;
	}
}

#endif