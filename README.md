# fast-dnn
fast dnn inference engine

## mnist forward perf data

| device           | hit count | average (ms) | min (ms) | max (ms) |
|:----------------:|:---------:|:------------:|:--------:|:--------:|
| opencv-cpu       | 10000     | 0.04         | 0.02     | 58.85    |
| opencv-gpu-nv    | 10000     | 0.81         | 0.53     | 14.31    |
| opencv-gpu-intel | 10000     | 1.18         | 0.78     | 19.91    |
| cpuref           | 10000     | 0.20         | 0.16     | 1.03     |
| cpuref*          | 10000     | 0.15         | 0.12     | 1.07     |

> cpuref*: cpuref with transposed 2d-array of mode data
