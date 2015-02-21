local citizen = {}

function citizen.checkIfLogin(connectId) 
    if nil == loggedCitizens[connectId] then
        return false
    end
    return true
end

function PUBCitizenLogin(connectId, email, password)
    if nil == email or nil == password then
        NTAddReplyRawString(connectId, "-请输入用户名或密码\r\n")
    end

    local citizen = util.db:select(
    'b_citizen', 
    'citizen_id,code,nickname,email', 
    {email=email, password=md5.sumhexa(password)})

    if not citizen then
        NTAddReplyRawString(connectId, "-用户名或密码错误\r\n")
        return
    end

    loggedCitizens[connectId] = citizen
    NTAddReplyRawString(connectId, "+登录成功\r\n")

end

return citizen
