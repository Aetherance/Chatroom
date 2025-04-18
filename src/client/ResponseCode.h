
/* 登录注册服务器响应码 */

/* 登录/注册 请求成功 */
#define USER_OK                       2000

/* 注册 */
#define USERNAME_ALREADY_EXISTS       4001 // 用户名已存在
#define EMAIL_ALREADY_REGISTERED      4002 // 邮箱已存在
#define VERIFICATION_CODE_INVALID     4003 // 验证码错误

/* 登录 */
#define USER_NOT_FOUND                4011 // 用户不存在
#define PASSWORD_INCORRECT            4012 // 密码错误


/* 客户端请求 action 字段定义 */
#define BIND_REQUEST "Requesting binding"
#define REGISTER_READY "Finishing registration"