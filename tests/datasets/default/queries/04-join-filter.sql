select * from employees, departments
where employees.id = departments.manager and employees.salary > 500;
