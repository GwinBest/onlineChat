# OnlineChat
## Build Tools
### [CMake](https://cmake.org/download/)
## Database
If you need to run the server, then download the [MySql](https://dev.mysql.com/downloads/workbench/)
## Getting Started
First, clone a repository
```
git clone https://github.com/GwinBest/OnlineChat.git
```
Then, make a build folder
```
cd OnlineChat
mkdir build
cd build
```
Run cmake 
```
cmake ..
```
To build all apps run
```
cmake --build .
```
Or, if you want to specify
```
cmake --build . --target (serverApp | clientApp)
```
# MySQL setup
First create a new connection with the following parametrs
```
Hostname: 127.0.0.1
Port: 3306
Username: root
Password: root
```
After connection, run the following code to create all necessary tables
``` SQL
CREATE SCHEMA `online_chat` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci ;


CREATE TABLE `online_chat`.`users` (
  `user_id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `user_name` VARCHAR(50) NOT NULL,
  `user_login` VARCHAR(50) NOT NULL,
  `user_password` BIGINT UNSIGNED NOT NULL,
  PRIMARY KEY (`user_id`),
  UNIQUE INDEX `user_id_UNIQUE` (`user_id` ASC) VISIBLE,
  UNIQUE INDEX `user_login_UNIQUE` (`user_login` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE `online_chat`.`chats` (
  `chat_id` INT NOT NULL AUTO_INCREMENT,
  `chat_participants` VARCHAR(105) NOT NULL,
  PRIMARY KEY (`chat_id`),
  UNIQUE INDEX `chat_id_UNIQUE` (`chat_id` ASC) VISIBLE,
  UNIQUE INDEX `chat_participants_UNIQUE` (`chat_participants` ASC) VISIBLE)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE `online_chat`.`messages` (
  `chat_id` INT NOT NULL,
  `author_id` INT NOT NULL,
  `message` VARCHAR(4096) NOT NULL)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE `online_chat`.`user_to_chat_relations` (
  `user_to_chat_relations` INT NOT NULL AUTO_INCREMENT,
  `user_id` INT NOT NULL,
  `chat_id` INT NOT NULL,
  PRIMARY KEY (`user_to_chat_relations`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COLLATE = utf8mb4_0900_ai_ci;

CREATE TABLE `online_chat`.`users_chat_names` (
  `users_chat_names` INT NOT NULL AUTO_INCREMENT,
  `user_id` INT NOT NULL,
  `chat_id` INT NOT NULL,
  `chat_name` VARCHAR(50) NOT NULL,
  PRIMARY KEY (`users_chat_names`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8mb4
COLLATE = utf8mb4_0900_ai_ci;
```

