# Core Development Team

The ClearCortex project was conceived, designed, developed, and managed by the following development team:

### [Malakalnawar](https://github.com/Malakalnawar)

* **Primary Role**: Developer and Project Manager
* **Technical Contributions**:
    * Initial conception of the project
    * Core data processing pipeline architecture
    * Extensive research surrounding the project and data processing algorithms used within ClearCortex
* **Management Contributions**:
    * Technical decision-making leadership
    * Development roadmap planning
    * Code review oversight
    * Technical meeting facilitation

### [Sightem](https://github.com/Sightem)

* **Primary Role**: Main Developer
* **Technical Contributions**:
    * Processing the raw EEG wave data
    * Visualizing the processed data
    * UI design and implementation
    * Architecture agnostic build system
    * Numerous frontend and backend pipeline optimizations
* **Management Contributions**:
    * Code design decisions regarding technical project architecture

## Project Events

* [1/10/25]: Project conception by Malakalnawar
* [1/17/25]: PKcalifornia joined the Core Development Team
* [1/17/25]: Project plan and design outline by Core Development Team
* [1/24/25]: Proposal sent to Alex Bauer (mentor) for DataJam and Dr. Erin Shaw (advisor) for Independent Study by - Malakalnawar
* [2/7/25]: Approval of project as DataJam participant and Independent Study at PCC
* [2/14/25]: Meeting with Core Development Team to discuss first steps in project plan
* [2/14/25]: Meeting with advisor and mentor
* [2/21/25]: Sightem joined the Core Development Team

## Code Development

* [2/27/25]: Add initial implementation of BrainViz project with data parsing by Sightem
    * Restructure the project
    * Fix the calls to println in main
    * Use the EEGData interface in main for demo

* [2/28/25]: Meeting with Core Development Team to discuss project plan
* [2/28/25]: Meeting with advisor and mentor
* [2/28/25]: Design and implementation of pre-processing (FFT) before visualization by Sightem
* [2/28/25]: Design and implementation of the initial user interface Malakalnawar
* [3/1/25]: Implementation of frequency analysis components by Sightem
    * Add BatchAnalyzer class for frequency analysis of EEG data
    * Logger.hpp for unified logging
    * Build system speed improvements
    * Main updated to demo an example of the analyzer
* [3/2/25]: UI framework integration
    * 401399a UI code (by Malakalnawar)
    * 2811dcf Sync (by Sightem)
    * f564c66 Prepare imgui+sfml (by Sightem)
    * 08d0620 Hopefully fix logger on mac (by Sightem)
    * 5534974 Add the imgui demo window (by Sightem)
    * The project can draw now
* [3/3/25]: Integration of UI and pre-processing by Sightem
    * Add electrode visualization components and documentation
* [3/5/25]: Event system implementation
    * Add event system for future GUI integration (by Sightem)
* [3/6/25]: Implementation of visualization and animation features
    * Actual drawing with animations (by Sightem)
    * Fix the interpolation in ElectrodeStateManager::update (by Sightem)

* [3/7/25]: Meeting with Core Development Team to discuss project plan
* [3/7/25]: Meeting with advisor and mentor
* [3/7/25]: Research and design of data processing pipeline by Malak Al Nawar and PKcalifornia

## Meeting Organization

Technical meetings for the ClearCortex project were scheduled, organized, and led by Malakalnawar. All technical decisions were made exclusively by the Core Development Team during these meetings.

## Project Scope Clarification

The entirety of the ClearCortex project, including:

* Conception and ideation
* Technical architecture and design
* Software development and implementation
* Testing and quality assurance
* Project management and coordination

Was performed exclusively by the Core Development Team members listed above, with advisory input from Alex Bauer and Dr. Erin Shaw.
No other individuals or entities participated in, managed, oversaw, or directed any technical, creative, or organizational aspects of this project.

## Identity Verification Note

While public attribution may use pseudonyms for privacy protection, the real identities of the Core Development Team are verifiable through cryptographic signatures in Github commits.

## Third-Party Components

This project utilizes the following external libraries and resources:

* GPL2 Library "kfrlib"
* MIT Library "fmtlib"
* MIT Library "Tessil/robin-map"
* MIT Library "ImGui"
* MIT Library "ImGui-SFML"
* ZLIB Library "SFML"

## Copyright Statement

Copyright © 2025 Original Developers of ClearCortex as identified in the Core Development Team section above.
