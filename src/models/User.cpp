#include "User.h"

// 定义角色常量
const QString User::ROLE_ADMIN = "admin";
const QString User::ROLE_ENGINEER = "engineer";
const QString User::ROLE_VIEWER = "viewer";

User::User()
    : userId(0)
{
}

User::User(int id, const QString &username, const QString &phone, const QString &role)
    : userId(id)
    , username(username)
    , phone(phone)
    , role(role)
{
}

User::~User()
{
}

bool User::isValid() const
{
    return userId > 0 && !username.isEmpty() && !role.isEmpty();
}
