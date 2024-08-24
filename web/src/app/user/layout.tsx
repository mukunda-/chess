// NextJS
import { redirect } from "next/navigation";

/// Ours - Styles
import styles from "./layout.module.css";

// Ours - Components
import { SiteNav } from "@/components/SiteNav";

// Ours - Auth
import { getServerSession } from "@/server/session";

export default async function UserLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  const session = await getServerSession();
  if (!session) {
    redirect("/");
  }

  return (
    <>
      <header className={styles["header"]}>
        <SiteNav />
      </header>
      <main className={styles["main"]}>{children}</main>
    </>
  );
}
