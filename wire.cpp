#include <algorithm>
#include <assert.h>
#include <functional>
#include <list>
#include <queue>
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>

using BasicValue = uint32_t;

// Denotes a callable with 0 arguments and returning no-value
using ActionProc = std::function<void()>;

struct Wire {
    // The current signal value of this wire
    BasicValue _signal = 0;
    // List of action procedures associated with this wire
    std::vector<ActionProc> _action_procs;
    // The wire can be given a name
    std::string _name;

    Wire(std::string name = "UNNAMED WIRE")
        : _name(name) {}

    // Current signal
    BasicValue signal() const { return _signal; }

    // Updates the signal value of this wire. If the value is different from
    // current value, calls each action proc.
    void set_signal(BasicValue v) {
        if (v != _signal) {
            _signal = v;

            if (_name == "s") {
                std::cout << "Sum = " << v;
            }

            if (_name == "c") {
                std::cout << "Carry = " << v;
            }

            call_each_action();
        }
    }

    // Adds a new action proc that will be called whenever the signal changes.
    // This is the 'registering'.
    void add_action(ActionProc action_proc) { _action_procs.push_back(action_proc); }

    void call_each_action() {
        for (auto &p : _action_procs) {
            p();
        }
    }
};

// Maintains action procs ordered by the timepoints at which they are to be
// invoked. Action procs are added and popped during simulation. Simulation
// ends when the timeline is empty. The `_origin` member denotes the current
// origin of the timeline. So procs still to be popped have a `t >= _origin`.
struct ProcTimeline {
    // Represents a discrete time-point.
    using TimePoint = uint32_t;

    // Multiple action procs can of course be scheduled to be invoked at the
    // same timepoint. So we collect them in a single `TimeSegment`.
    struct TimeSegment {
        TimePoint t;
        // The procs to be invoked at this timepoint, ordered by
        std::queue<ActionProc> procs;
    };

    // Timepoint of the last popped action proc.
    TimePoint _origin = 0;
    // The timeline. TimeSegments must always be ordered by increasing `t`.
    // Storing it in an std::list since we pop from front, but add to back.
    std::list<TimeSegment> _time_segments;

    // Gets the global timeline instance
    static ProcTimeline &instance() {
        static ProcTimeline agenda;
        return agenda;
    }

    bool empty() const { return _time_segments.size() == 0; }

    TimePoint current_origin() const { return _origin; }

    // Adds a new action procedure `proc` to be invoked at the given timepoint `t`.
    void add(ActionProc proc, TimePoint t) {
        auto it = _time_segments.begin();
        auto end = _time_segments.end();
        while (true) {
            if (it == end || it->t > t) {
                // Create new segment in between, or append new segment
                _time_segments.emplace(it, TimeSegment());
                _time_segments.front().t = t;
                _time_segments.front().procs.push(proc);
                break;
            } else if (it->t == t) {
                // Found exact timepoint
                it->procs.push(std::move(proc));
                break;
            } else {
                ++it;
            }
        }
    }

    // Pops one of the procs at the closest timepoint
    ActionProc pop() {
        assert(!empty() && "Timeline doesn't have any actions to pop!");
        TimeSegment &front_segment = _time_segments.front();

        _origin = front_segment.t;
        ActionProc proc = front_segment.procs.front();

        front_segment.procs.pop();

        if (front_segment.procs.size() == 0) {
            _time_segments.pop_front();
        }

        return proc;
    }
};

// Schedules the given action to be called after `delay` timepoints
static inline void call_after_delay(ProcTimeline::TimePoint delay, ActionProc proc) {
    auto &timeline = ProcTimeline::instance();
    timeline.add(std::move(proc), delay + timeline.current_origin());
}

static inline void simulate() {
    auto &timeline = ProcTimeline::instance();

    while (!timeline.empty()) {
        auto proc = timeline.pop();
        proc();
    }
}

// Test

// We need a place to store all the wires we are creating so that we can free
// them in one go (by returning from main). Also, we are using pointers to
// wires in the action procs. So we can't directly store in this vector lest
// the pointers get invalidated. Reference counting could be used. We'll see.
static std::vector<Wire *> wire_storage;

Wire *new_wire(std::string name = "UNNAMED WIRE") {
    wire_storage.push_back(new Wire(name));
    return wire_storage.back();
}

void free_wire_storage() {
    for (Wire *w : wire_storage) {
        delete w;
    }
}

void make_and_gate(Wire *in1, Wire *in2, Wire *out) {
    // The captured wires are how the proc notes down the input and output wires.
    auto proc = [in1, in2, out]() {
        if (in1->signal() && in2->signal()) {
            out->set_signal(BasicValue(1));
        } else {
            out->set_signal(BasicValue(0));
        }
    };

    // Add the proc to the timeline
    call_after_delay(1, proc);

    // The proc then gets registered to the input wires
    in1->add_action(proc);
    in2->add_action(proc);
}

void make_or_gate(Wire *in1, Wire *in2, Wire *out) {
    auto proc = [in1, in2, out]() {
        if (in1->signal() || in2->signal()) {
            out->set_signal(BasicValue(1));
        } else {
            out->set_signal(BasicValue(0));
        }
    };

    call_after_delay(1, proc);

    in1->add_action(proc);
    in2->add_action(proc);
}

void make_inverter(Wire *in, Wire *out) {
    auto proc = [=]() { out->set_signal(!in->signal()); };
    call_after_delay(1, proc);
    in->add_action(proc);
}

void make_half_adder(Wire *a, Wire *b, Wire *s, Wire *c) {
    Wire *d = new_wire();
    Wire *e = new_wire();
    make_or_gate(a, b, d);
    make_and_gate(a, b, c);
    make_inverter(c, e);
    make_and_gate(d, e, s);
}

int main() {
    Wire *a = new_wire("a");
    Wire *b = new_wire("b");
    Wire *s = new_wire("s");
    Wire *c = new_wire("c");

    make_half_adder(a, b, s, c);

    a->set_signal(1);
    b->set_signal(1);


    simulate();

    free_wire_storage();
}
