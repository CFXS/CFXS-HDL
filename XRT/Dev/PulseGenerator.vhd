LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;
LIBRARY CFXS;
USE CFXS.Utils.ALL;

-- Edge triggered pulse generator
ENTITY PulseGenerator IS
    GENERIC (
        IDLE_OUTPUT : STD_LOGIC; -- Idle output state - pulse state is inverted IDLE_OUTPUT
        PULSE_LENGTH : NATURAL; -- Length of a pulse in clock cycles
        CLOCK_EDGE : STD_LOGIC := '1'; -- Process clock edge
        TRIGGER_EDGE : STD_LOGIC := '1'; -- Default trigger on transition from 0 to 1
        BOTH_EDGES : STD_LOGIC := '0' -- Generate pulse on both trigger edges
    );
    PORT (
        clock : IN STD_LOGIC;
        trigger : IN STD_LOGIC; -- Trigger pulse input
        output : OUT STD_LOGIC -- Output pulse
    );
END ENTITY;

ARCHITECTURE RTL OF PulseGenerator IS
    -- Pulse clock counter
    SIGNAL reg_Counter : unsigned(HighBit(PULSE_LENGTH) DOWNTO 0) := (OTHERS => '0');
    SIGNAL reg_LastTrigger : STD_LOGIC := NOT TRIGGER_EDGE;
BEGIN
    PROCESS (ALL)
    BEGIN
        IF rising_edge(clock) THEN
            IF trigger /= reg_LastTrigger THEN
                IF BOTH_EDGES = '1' OR trigger = TRIGGER_EDGE THEN
                    reg_Counter <= to_unsigned(PULSE_LENGTH, reg_Counter'length);
                END IF;
                reg_LastTrigger <= trigger;
            END IF;

            IF reg_Counter /= 0 THEN
                reg_Counter <= reg_Counter - 1;
            END IF;
        END IF;
    END PROCESS;

    output <= IDLE_OUTPUT WHEN reg_Counter = 0 ELSE
        NOT IDLE_OUTPUT;
END ARCHITECTURE;