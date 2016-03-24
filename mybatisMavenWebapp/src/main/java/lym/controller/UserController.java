package lym.controller;

import javax.servlet.http.HttpServletRequest;

import lym.model.User;
import lym.service.UserServiceI;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;

@Controller
@RequestMapping("/userController")
public class UserController {
	
	private UserServiceI userService;
	
	public UserServiceI getUserService() {
		return userService;
	}

	@Autowired
	public void setUserService(UserServiceI userService) {
		this.userService = userService;
	}

	@RequestMapping("/showUser")
	public String showUser(String id, HttpServletRequest request){
		User user = userService.getUserById(id);
		request.setAttribute("user", user);
		return "showUser";
	}
}
