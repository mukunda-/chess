"use client";

// NextJS
import Link from "next/link";
import { usePathname } from "next/navigation";

// Ours - Styles
import styles from "./SiteNav.module.css";

type NavLinkItemProps = {
  to: string;
  currentPath: string;
  children: React.ReactNode;
};

function NavLinkItem({ to, children, currentPath }: NavLinkItemProps) {
  const isActive = currentPath === to;

  return (
    <li className={styles["nav__item"]}>
      <Link
        className={styles[`nav__item__link${isActive ? "--active" : ""}`]}
        href={to}
      >
        {children}
      </Link>
    </li>
  );
}

export function SiteNav() {
  const pathname = usePathname();

  return (
    <nav className={styles["nav"]}>
      <ul>
        <NavLinkItem to="/user" currentPath={pathname}>
          Home
        </NavLinkItem>
      </ul>
    </nav>
  );
}
