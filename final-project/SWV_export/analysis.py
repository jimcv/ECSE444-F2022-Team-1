import matplotlib.pyplot as plt
import numpy as np

itm_uart_done = '1'
itm_engine_update = '2'

mode = "rtos"
logpath = f"SWV_Trace_Log_{mode}.txt"

uart_cnt = 0
uart_cycles = []
uart_count = []
engine_cnt = 0
engine_cycles = []
engine_count = []

with open(logpath) as f:
    lines = f.readlines()

    for line in lines:
        tokens = [(token.strip('"')) for token in line.split(';') if len(token) != 0]

        if (tokens[1] != 'ITM Port 31'):
            continue

        if tokens[2] == itm_uart_done:
            uart_cnt += 1
            uart_count.append(int(uart_cnt))
            uart_cycles.append(int(tokens[3]))
        elif tokens[2] == itm_engine_update:
            engine_cnt += 1
            engine_count.append(int(engine_cnt))
            engine_cycles.append(int(tokens[3]))

plt.plot(uart_cycles, uart_count, label='RTOS UART completions')
plt.plot(engine_cycles, engine_count, label='RTOS Engine updates')

uart_throughput = (uart_count[-1] - uart_count[0]) / (uart_cycles[-1] - uart_count[0]) * 1e8
engine_throughput = (engine_count[-1] - engine_count[0]) / (engine_cycles[-1] - engine_count[0]) * 1e8
print(f'With RTOS the average UART throughput was {uart_throughput} frames per 1e8 cycles')
print(f'With RTOS the average engine throughput was {engine_throughput} updates per 1e8 cycles')

plt.title('Number of events with RTOS execution')
plt.xlabel('Cycles')
plt.ylabel('Number of events')
plt.legend(bbox_to_anchor=(0.05, 0.95), loc='upper left', borderaxespad=0.)
plt.show()

mode = "serial"
logpath = f"SWV_Trace_Log_{mode}.txt"

uart_cnt = 0
uart_cycles = []
uart_count = []
engine_cnt = 0
engine_cycles = []
engine_count = []

with open(logpath) as f:
    lines = f.readlines()

    for line in lines:
        tokens = [(token.strip('"')) for token in line.split(';') if len(token) != 0]

        if (tokens[1] != 'ITM Port 31'):
            continue

        if tokens[2] == itm_uart_done:
            uart_cnt += 1
            uart_count.append(int(uart_cnt))
            uart_cycles.append(int(tokens[3]))
        elif tokens[2] == itm_engine_update:
            engine_cnt += 1
            engine_count.append(int(engine_cnt))
            engine_cycles.append(int(tokens[3]))

plt.plot(uart_cycles, uart_count, label='Serial UART completions')
plt.plot(engine_cycles, engine_count, label='Serial Engine updates')

uart_throughput = (uart_count[-1] - uart_count[0]) / (uart_cycles[-1] - uart_count[0]) * 1e8
engine_throughput = (engine_count[-1] - engine_count[0]) / (engine_cycles[-1] - engine_count[0]) * 1e8
print(f'With serial execution the average UART throughput was {uart_throughput} frames per 1e8 cycles')
print(f'With serial execution the average engine throughput was {engine_throughput} updates per 1e8 cycles')

plt.title('Number of events with RTOS and serial execution')
plt.xlabel('Cycles')
plt.ylabel('Number of events')
plt.legend(bbox_to_anchor=(0.05, 0.95), loc='upper left', borderaxespad=0.)
plt.show()
