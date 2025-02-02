#pragma once

#define APT 3.0 // Global linearization point
#define XTH 1.2
#define XEND 2

// The BPR travel cost function, relating the travel time on an edge to the flow on this edge.
class BprFunction {
public:
	// Constructs a BPR function.
	BprFunction(const Graph& graph) : graph(graph) {}

	// Returns the travel time on edge e, given the flow x on e.
	double operator()(const int e, const double x) const {
		const double tmp = x / graph.capacity(e);
		return graph.freeTravelTime(e) * (1 + 0.15 * tmp * tmp * tmp * tmp);
	}

	// Returns the derivative of e's travel cost function at x.
	double derivative(const int e, const double x) const {
		const double tmp = x / graph.capacity(e);
		return graph.freeTravelTime(e) * 0.15 * 4 * tmp * tmp * tmp / graph.capacity(e);
	}

	// Returns the derivative of e's travel cost function at x.
	double secondDerivative(const int e, const double x) const {
		const double tmp = x / graph.capacity(e);
		return graph.freeTravelTime(e) * 0.15 * 4 * 3 * tmp * tmp / (graph.capacity(e) * graph.capacity(e));
	}
	
	// Returns the antiderivative of e's travel cost function at x.
	double antiderivative(const int e, const double x) const {
		const double tmp = x / graph.capacity(e);
		return graph.freeTravelTime(e) * (x + 0.15 * x * tmp * tmp * tmp * tmp / (4 + 1));
	}

	// Returns the integral of e's travel cost function from 0 to b.
	double integral(const int e, const double b) const {
		return antiderivative(e, b) - antiderivative(e, 0);
	}	

private:
	const Graph& graph; // The graph on whose edges we operate.
};
