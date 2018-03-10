namespace rat
{

template <typename... Ts>
ModulesHolder<Ts...>::ModulesHolder()
{
	((modulePtr_v<Ts> = std::get<Held_t<Ts>>(_modules).getPtr()), ...);
}

template <typename... Ts>
template <typename U, typename... Us>
void ModulesHolder<Ts...>::initModule(Us&&... args)
{
	std::get<Held_t<U>>(_modules).init(std::forward<Us>(args)...);
}

template <typename... Ts>
template <typename U>
U& ModulesHolder<Ts...>::getModule()
{
	return std::get<Held_t<U>>(_modules).getRef();
}

template <typename... Ts>
template <typename U>
const U& ModulesHolder<Ts...>::getModule() const
{
	return std::get<Held_t<U>>(_modules).getRef();
}

}