# traffic-monitoring

Traffic Monitoring is an application designed to make traffic more organized and efficient,
ensuring smoother journeys for drivers.

Drivers are allowed to connect and receive traffic assistance.
Also, they can receive information on the weather, fuel prices at nearby stations, and various sports events that are
happening in the city they are driving through. In this application, drivers can alert and be
alerted by other registered users about accidents. Moreover, drivers can receive warnings based on
their speed, which is automatically updated every minute during their journey, helping them drive
more safely and responsibly.

![seq_diagram](https://github.com/user-attachments/assets/5f01cff2-2b65-4b19-9832-32784aab32e2)

For this project, I chose to implement a concurrent TCP (Transmission Control Protocol) server that creates a thread for each connected client. I chose this protocol over others because
TCP ensures accurate information transmission by establishing a virtual connection between the
sender and receiver, which is important for my project, since drivers rely on precise and reliable data.

In this application, all data is stored in a MySQL database.

![db_diagram](https://github.com/user-attachments/assets/deb46224-41da-4c73-918d-f7dc5f1c6fc3)

