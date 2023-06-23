FROM ubuntu

LABEL author="Ediseev Oleg"

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools -y
RUN apt-get install build-essential -y

WORKDIR /221-329_Ediseev_Oleg
COPY *.cpp .
COPY *.h .
COPY *.pro .

RUN qmake echoServer.pro
RUN make 

ENTRYPOINT ["./gameServer"]