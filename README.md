# WEBSERV | C++, Unix Sockets

## Overview

This project is an HTTP server developed in C++ that leverages Unix sockets for high-performance communication. It was created in collaboration with a team, with a primary focus on optimizing server efficiency and response times.

## Features

- **Poll Functionality**: The server utilizes the `poll` function to efficiently handle incoming requests and generate timely responses. This ensures a streamlined flow of data between the server and clients.

- **Common Gateway Interface (CGI)**: WEBSERV supports CGI, enabling the creation of interactive web pages. This feature provides dynamic content generation, enhancing the user experience.

- Responsible for debugging errors and ensuring the smooth operation of the server.

## Getting Started

To get started with WEBSERV, follow these steps:

1. Clone the repository to your local machine:

```bash
git clone git@github.com:SOverFLow/http-web-server.git

2. Navigate to the project directory

3. Build the project using the make command: make

4. Launch the server: ./webserv


```

The server will now be running locally.

    Open your web browser and navigate to http://localhost:1337 to access the server.

Usage

    - Request Handling: The server listens for incoming HTTP requests and efficiently processes them, ensuring timely responses.

    - Interactive Web Pages: Utilize the CGI functionality to create dynamic and interactive web pages for enhanced user engagement.
