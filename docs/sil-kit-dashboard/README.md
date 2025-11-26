
# SIL Kit Dashboard: General Usage and Information

This document provides general instructions and information for using the SIL Kit Dashboard. The dashboard is a graphical tool for monitoring and visualizing the activity of the SIL Kit participants, offering real-time insights into communication, participant status, and assisting with debugging and understanding SIL Kit-based setups.

## Download the Dashboard

Download the latest SIL Kit Dashboard release from the Vector website: [Vector SIL Kit Dashboard](https://vector.com/sil-kit-dashboard)

After downloading, extract the archive to a convenient location on your system.

> To use the `metrics` feature, you must have Vector SIL Kit 5.0.2 and Vector SIL Kit Dashboard 1.1.0 (or newer versions).

## Install the Dashboard

Installation instructions and requirements for the SIL Kit Dashboard are provided in the extracted directory:
``/path/to/extracted/vector-sil-kit-dashboard-x.y.z/<platform>/README.md``

> Replace `<platform>` with one of the following, depending on your system: win-x64, linux-x64, or portable.

## Start the Dashboard

1. Start the SIL Kit Registry with dashboard support:
  ```
  /path/to/SilKit-x.y.z-$platform/SilKit/bin/sil-kit-registry --listen-uri 'silkit://0.0.0.0:8501' --dashboard-uri http://localhost:8082
  ```

2. Start the adapter and demo devices as described above. For more details on configuring SIL Kit participants, see the [SIL Kit documentation](https://vectorgrp.github.io/sil-kit-docs/troubleshooting/troubleshooting.html).

3. For installed versions (Windows or Linux), the dashboard service starts automatically after installation. For the portable version, start the dashboard manually.

Once the dashboard is running, open your web browser and go to [http://localhost:8080/dashboard](http://localhost:8080/dashboard) to access the web interface.

> To avoid additional configuration effort, it is recommended to run the SIL Kit Dashboard on the same machine as the SIL Kit Registry.

## Features and Usage

- **Participant Overview:**
  View all connected SIL Kit participants, including their names and status.

- **Data Flow Visualization:**
  Visualize communication between participants, including published and subscribed topics and networks.

- **Live Monitoring:**
  Monitor participant connections, disconnections, and other events in real time.

- **Diagnostics:**
  Identify issues such as missing participants, unexpected message patterns, or connection problems using the dashboard.

## Additional Resources

For more information, refer to the official SIL Kit documentation or the dashboard's help section.
